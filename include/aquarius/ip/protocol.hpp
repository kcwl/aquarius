#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/module/handler_channel.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/virgo/http_null_protocol.hpp>
#include <boost/url.hpp>
#include <ranges>
#include <span>

namespace aquarius
{
	template <typename T>
	struct handler_tag_traits;

	struct tcp_protocol
	{
		using socket = boost::asio::ip::tcp::socket;

		using endpoint = boost::asio::ip::tcp::endpoint;

		using acceptor = boost::asio::ip::tcp::acceptor;

		using resolver = boost::asio::ip::tcp::resolver;

		using no_delay = boost::asio::ip::tcp::no_delay;

		using keep_alive = boost::asio::socket_base::keep_alive;

		template <typename Socket, typename Dura, typename Session>
		static auto accept(Socket& socket, Dura timeout, std::shared_ptr<Session> session_ptr) -> awaitable<error_code>
		{
			auto ec = co_await socket.accept(timeout);

			if (!ec)
			{
				for (;;)
				{
					flex_buffer buffer{};

					ec = co_await recv(session_ptr, buffer);

					if (ec)
					{
						break;
					}

					auto router = binary_parse{}.from_datas<std::string>(buffer);

					XLOG_INFO() << "[accept] parse protocol router: " << router;

					co_spawn(
						session_ptr->get_executor(),
						[&, r = std::move(router)] -> awaitable<void>
						{
							auto result = co_await mpc_publish(std::move(r), buffer);

							if (!result.has_value())
							{
								co_return;
							}

							co_await session_ptr->async_send(std::move(*result));
						},
						detached);
				}
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[accept] error: " << ec.what();
			}

			co_return ec;
		}

		template <typename Session>
		static auto query(std::shared_ptr<Session> session_ptr, error_code& ec) -> awaitable<flex_buffer>
		{
			ec = error_code{};

			for (;;)
			{
				flex_buffer buffer{};

				ec = co_await recv(session_ptr, buffer);
				if (ec)
				{
					break;
				}

				co_return std::move(buffer);
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[query buffer] error: " << ec.what();
			}

			co_return flex_buffer{};
		}

	private:
		template <typename Session>
		static auto recv(std::shared_ptr<Session> session_ptr, flex_buffer& buffer) -> awaitable<error_code>
		{
			uint32_t packet_length{};

			constexpr auto len = sizeof(packet_length);

			auto ec = co_await session_ptr->async_read(buffer, len);

			if (!ec)
			{
				buffer.sgetn((char*)&packet_length, sizeof(packet_length));

				ec = co_await session_ptr->async_read(buffer, packet_length);
			}

			co_return ec;
		}
	};

	struct http_protocol
	{
		using socket = boost::asio::ip::tcp::socket;

		using endpoint = boost::asio::ip::tcp::endpoint;

		using acceptor = boost::asio::ip::tcp::acceptor;

		using resolver = boost::asio::ip::tcp::resolver;

		using no_delay = boost::asio::ip::tcp::no_delay;

		using keep_alive = boost::asio::socket_base::keep_alive;

		constexpr static auto two_crlf = "\r\n\r\n"sv;

		constexpr static auto crlf = "\r\n"sv;

		constexpr static std::size_t header_part = 3;

		template <typename Socket, typename Session, typename Dura>
		static auto accept(Socket& socket, Dura timeout, std::shared_ptr<Session> session_ptr) -> awaitable<error_code>
		{
			auto ec = co_await socket.accept(timeout);

			for (;;)
			{
				flex_buffer buffer{};

				ec = co_await session_ptr->async_read_util(buffer, two_crlf);

				if (ec)
				{
					break;
				}

				auto header_line = std::string((char*)buffer.data().data(), buffer.size());

				header_line = header_line.substr(0, header_line.find_first_of(crlf));

				buffer.consume(header_line.size() + crlf.size());

				auto [method, url, version] = parse_command_line<true>(std::span<char>(header_line), ec);

				if (method == virgo::http_method::get)
				{
					buffer.sputn(url->params().buffer().data(), url->params().buffer().size());
				}

				std::string router(std::string_view(url->path().data(), url->path().size()));

				ec = co_await recv(session_ptr, buffer);

				co_spawn(
					session_ptr->get_executor(),
					[&, r = std::move(router)] -> awaitable<void>
					{
						auto resp_buf = co_await mpc_publish(std::move(r), buffer, static_cast<int>(version));

						if (!resp_buf.has_value())
						{
							co_return;
						}

						co_await session_ptr->async_send(std::move(*resp_buf));
					},
					detached);
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[accept] error: " << ec.what();
			}

			co_return ec;
		}

		template <typename Session>
		static auto query(std::shared_ptr<Session> session_ptr, error_code& ec, std::size_t seq = 0)
			-> awaitable<flex_buffer>
		{
			flex_buffer buffer{};

			ec = co_await session_ptr->async_read_util(buffer, two_crlf);

			if (ec)
			{
				co_return buffer;
			}

			auto header_line = std::string((char*)buffer.data().data(), buffer.size());

			header_line = header_line.substr(0, header_line.find_first_of(crlf));

			buffer.consume(header_line.size() + crlf.size());

			auto [version, status] = parse_command_line<false>(std::span<char>(header_line), ec);

			ec = co_await recv(session_ptr, buffer);

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[query buffer] error: " << ec.what();
			}

			co_return std::move(buffer);
		}

	private:
		template <typename Session>
		static auto recv(std::shared_ptr<Session> session_ptr, flex_buffer& buffer) -> awaitable<error_code>
		{
			// auto content_type = hf.find("content-type");

			// if (!content_type.empty() && content_type != "application/json" &&
			// method
			// == virgo::http_method::get)
			//{
			//    router = __http_source_handler__;
			// }
			// else if (method == virgo::http_method::get)
			//{
			//    if (!path.empty())
			//    {
			//        buffer.sputn(path.data(), path.size());
			//    }
			// }
			// else if (method == virgo::http_method::options)
			//{
			//    router = __http_options_handler__;
			// }

			// if (ec)
			//{
			//	break;
			// }

			error_code ec{};

			std::size_t content_length = 0;

			std::string header_str((char*)buffer.data().data(), buffer.size());

			auto pos = header_str.find("Content-Length");
			if (pos != std::string::npos)
			{
				auto end_pos = header_str.find_first_of("\r\n", pos);
				if (end_pos != std::string::npos)
				{
					auto start = header_str.find_first_of(':', pos);
					auto key = header_str.substr(pos, start - pos);
					if (key == "Content-Length")
					{
						std::string content_length_str = header_str.substr(start + 2, end_pos - start - 2);

						std::stringstream ss{};
						ss << content_length_str;

						ss >> content_length;
					}
				}
			}

			if (content_length > 0)
			{
				auto remain_size = static_cast<int64_t>(content_length - buffer.size());

				if (remain_size > 0)
				{
					ec = co_await session_ptr->async_read(buffer, remain_size);
				}
			}

			co_return ec;
		}

		template <bool Server>
		static auto parse_command_line(std::span<char> header_span, error_code& ec)
		{
			using result_t = std::conditional_t<
				Server,
				std::tuple<virgo::http_method, boost::system::result<boost::urls::url_view>, virgo::http_version>,
				std::tuple<virgo::http_version, virgo::http_status>>;

			ec = boost::asio::error::eof;

			auto sp = header_span | std::views::split(' ');

			auto size = std::ranges::distance(sp.begin(), sp.end());

			if (size != header_part)
			{
				return result_t{};
			}

			auto iter = sp.begin();

			if constexpr (Server)
			{
				auto method = virgo::from_string_method(std::string_view(*iter++));

				if (method == virgo::http_method::none)
				{
					return result_t{};
				}

				auto url_result = boost::urls::parse_origin_form(std::string_view(*iter++));
				if (url_result.has_error())
				{
					return result_t{};
				}

				auto version = virgo::from_version_string(std::string_view(*iter));

				if (version == virgo::http_version::unknown)
				{
					return result_t{};
				}

				ec = error_code{};

				return std::make_tuple(method, url_result, version);
			}
			else
			{
				auto version = virgo::from_version_string(std::string_view(*iter++));

				auto status = static_cast<virgo::http_status>(std::atoi(std::string_view(*iter++).data()));

				ec = error_code{};

				return std::make_tuple(version, status);
			}
		}
	};

	struct udp_protocol
	{
		struct null_acceptor
		{};

		struct null_delay
		{
			null_delay(bool)
			{
				XLOG_ERROR() << "udp not support delay algrothm and operator is not effective";
			}
		};

		using socket = boost::asio::ip::udp::socket;

		using endpoint = boost::asio::ip::udp::endpoint;

		using acceptor = null_acceptor;

		using resolver = boost::asio::ip::udp::resolver;

		using no_delay = null_delay;

		using keep_alive = boost::asio::socket_base::keep_alive;
	};
} // namespace aquarius