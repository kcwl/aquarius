#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/module/channel_module.hpp>
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

		template <typename Selector, typename Socket, typename Dura, typename Session>
		static auto accept(Socket& socket, Dura timeout, std::shared_ptr<Session> session_ptr) -> awaitable<error_code>
		{
			auto ec = co_await socket.accept(timeout);

			if (!ec)
			{
				for (;;)
				{
					flex_buffer buffer{};

					uint32_t seq{};

					ec = co_await recv(session_ptr, buffer, seq);

					if (ec)
					{
						break;
					}

					auto router = binary_parse{}.from_datas<std::string>(buffer);

					XLOG_INFO() << "[accept] parse protocol router: " << router;

					co_spawn(
						session_ptr->get_executor(),
						[&] -> awaitable<void>
						{
							auto result = co_await mpc_publish(router, buffer);

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
		static auto query(std::shared_ptr<Session> session_ptr, std::size_t seq_number, error_code& ec)
			-> awaitable<flex_buffer>
		{
			flex_buffer buffer{};

			ec = error_code{};

			static std::map<std::size_t, flex_buffer> buffer_controller_;

			auto iter = buffer_controller_.find(seq_number);

			if (iter != buffer_controller_.end())
			{
				XLOG_INFO() << "[query from controll buffer] seq_number: " << seq_number;

				buffer = std::move(iter->second);

				buffer_controller_.erase(iter);
			}
			else
			{
				for (;;)
				{
					uint32_t seq{};
					ec = co_await recv(session_ptr, buffer, seq);
					if (ec)
					{
						break;
					}

					if (seq == seq_number)
					{
						co_return buffer;
					}

					buffer.pubseekoff(sizeof(uint32_t), std::ios::cur);

					XLOG_INFO() << "[query controll buffer] seq_number: " << seq << ", wait seq: " << seq_number;

					buffer_controller_.emplace(seq, std::move(buffer));
				}
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[query buffer] error: " << ec.what();
			}

			co_return buffer;
		}

	private:
		template <typename Session>
		static auto recv(std::shared_ptr<Session> session_ptr, flex_buffer& buffer, uint32_t& req)
			-> awaitable<error_code>
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

		template <typename Selector, typename Socket, typename Session, typename Dura>
		static auto accept(Socket& socket, Dura timeout, std::shared_ptr<Session> session_ptr) -> awaitable<error_code>
		{
			auto ec = co_await socket.accept(timeout);

			for (;;)
			{
				flex_buffer buffer{};
				ec = co_await session_ptr->async_read_util(buffer, crlf);

				if (ec)
				{
					break;
				}

				auto header_line = std::string((char*)buffer.data().data(), buffer.size());

				auto [method, url, version] = parse_command_line<true>(std::span<char>(header_line), ec);

				std::string router(std::string_view(url->path().data(), url->path().size()));

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

				ec = co_await session_ptr->async_read_util(buffer, two_crlf);

				if (ec)
				{
					break;
				}

				std::size_t content_length = 0;

				std::string header_str((char*)buffer.data().data(), buffer.size());

				auto pos = header_str.find("Content-Length");
				if (pos != std::string::npos)
				{
					auto end_pos = header_str.find_first_of("\r\n", pos);
					if (end_pos != std::string::npos)
					{
						auto start = header_str.find_first_of(':', pos);

						if (header_str.substr(pos, end_pos - pos) == "Content-Length")
						{
							std::string content_length_str = header_str.substr(start + 1, end_pos - start - 1);

							std::stringstream ss{};
							ss << content_length_str;

							ss >> content_length;
						}
					}
				}

				if (content_length != 0)
				{
					auto remain_size = static_cast<int64_t>(content_length - buffer.size());

					if (remain_size > 0)
					{
						ec = co_await session_ptr->async_read(buffer, remain_size);
					}
				}

				if (ec)
				{
					break;
				}

				co_spawn(
					session_ptr->get_executor(),
					[&]->awaitable<void>
					{
						auto resp_buf = co_await mpc_publish(router, buffer, static_cast<int>(version));

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
		static auto query(std::shared_ptr<Session> session_ptr, std::size_t seq_number, error_code& ec)
			-> awaitable<flex_buffer>
		{
			flex_buffer buffer{};

			static std::map<std::size_t, flex_buffer> buffer_controller_;

			auto iter = buffer_controller_.find(seq_number);

			if (iter != buffer_controller_.end())
			{
				XLOG_INFO() << "[query from controll buffer] seq_number: " << seq_number;

				buffer = std::move(iter->second);

				buffer_controller_.erase(iter);
			}
			else
			{
				for (;;)
				{
					auto size = buffer.size();

					ec = co_await session_ptr->async_read_util(buffer, crlf);

					if (ec)
					{
						break;
					}

					auto header_line = std::string((char*)buffer.data().data(), buffer.size());

					auto [version, status] = parse_command_line<false>(std::span<char>(header_line), ec);

					virgo::http_null_response resp{};

					resp.version(static_cast<int32_t>(version));
					resp.result(static_cast<int32_t>(status));

					ec = co_await session_ptr->async_read_util(buffer, two_crlf);

					if (ec)
					{
						co_return flex_buffer{};
					}

					resp.header().deserialize(buffer);

					auto content_length = resp.header().content_length();

					if (content_length != 0)
					{
						auto remain_size = static_cast<int64_t>(content_length - buffer.size());

						if (remain_size > 0)
						{
							ec = co_await session_ptr->async_read(buffer, remain_size);
						}
					}

					if (ec)
					{
						break;
					}

					if (resp.header().sequence() != seq_number)
					{
						XLOG_INFO() << "[query controll buffer] seq_number: " << resp.header().sequence()
									<< ", wait seq: " << seq_number;

						buffer.pubseekoff(size, std::ios::beg);

						buffer_controller_.emplace(resp.header().sequence(), std::move(buffer));
						continue;
					}

					break;
				}
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[query buffer] error: " << ec.what();
			}

			co_return std::move(buffer);
		}

	private:
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