#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/basic_client.hpp>
#include <aquarius/basic_server.hpp>
#include <aquarius/basic_session.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/adaptor/raw_adaptor.hpp>
#include <aquarius/ip/adaptor/ssl_adaptor.hpp>
#include <aquarius/ip/http/http_options_handler.hpp>
#include <aquarius/module/handler_channel.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <boost/url.hpp>
#include <ranges>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	struct http
	{
		using socket = asio::ip::tcp::socket;

		using endpoint = asio::ip::tcp::endpoint;

		using acceptor = asio::ip::tcp::acceptor;

		using resolver = asio::ip::tcp::resolver;

		using no_delay = asio::ip::tcp::no_delay;

		using keep_alive = asio::socket_base::keep_alive;

		constexpr static auto two_crlf = "\r\n\r\n"sv;

		constexpr static auto crlf = "\r\n"sv;

		constexpr static std::size_t header_part = 3;

		template <typename Session>
		static auto accept(std::shared_ptr<Session> session_ptr) -> asio::awaitable<error_code>
		{
			error_code ec{};

			for (;;)
			{
				flex_buffer buffer{};

				std::size_t header_line_end_pos{};

				ec = co_await session_ptr->async_read_util(buffer, two_crlf, header_line_end_pos);

				if (ec)
				{
					break;
				}

				auto header_line = std::string_view((char*)buffer.data().data(), header_line_end_pos);

				header_line = header_line.substr(0, header_line.find_first_of(crlf));

				buffer.consume(header_line.size() + crlf.size());

				auto [method, url, version] = parse_command_line<true>(header_line, ec);

				if (ec)
				{
					co_return ec;
				}

				if (method == http_method::options)
				{
					auto resp = co_await mpc_http_options(buffer);

					co_await session_ptr->async_send(std::move(*resp));
				}
				else
				{
					if (method == http_method::get)
					{
						buffer.sputn(url->params().buffer().data(), url->params().buffer().size());
					}

					std::string router(std::string_view(url->path().data(), url->path().size()));

					ec = co_await recv(session_ptr, buffer);

					asio::co_spawn(
						session_ptr->get_executor(),
						[&, r = std::move(router)] -> asio::awaitable<void>
						{
							auto resp_buf = co_await mpc_publish(std::move(r), buffer);

							if (!resp_buf.has_value())
							{
								co_return;
							}

							co_await session_ptr->async_send(std::move(*resp_buf));
						},
						asio::detached);
				}
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[accept] error: " << ec.what();
			}

			co_return ec;
		}

		template <typename Session>
		static auto query(std::shared_ptr<Session> session_ptr)
			-> asio::awaitable<std::expected<flex_buffer, error_code>>
		{
			flex_buffer buffer{};

			std::size_t end_pos{};

			auto ec = co_await session_ptr->async_read_util(buffer, two_crlf, end_pos);

			if (ec)
			{
				co_return buffer;
			}

			ec = co_await recv(session_ptr, buffer);

			if (ec)
			{
				XLOG_ERROR() << "[query buffer] error: " << ec.what();
				co_return flex_buffer{};
			}

			std::string_view header_line((char*)buffer.data().data(), buffer.size());
			auto pos = header_line.find_first_of(crlf);

			auto [version, status] = parse_command_line<false>(header_line.substr(0, pos), ec);

			if (ec)
			{
				co_return flex_buffer{};
			}

			if (status != http_status::ok)
			{
				co_return flex_buffer{};
			}

			buffer.consume(pos);

			co_return std::move(buffer);
		}

	private:
		template <typename Session>
		static auto recv(std::shared_ptr<Session> session_ptr, flex_buffer& buffer) -> asio::awaitable<error_code>
		{
			error_code ec{};

			std::size_t content_length = 0;

			std::string_view header_str((char*)buffer.data().data(), buffer.size());

			auto content_length_str = parse_key(header_str, "Content-Length");

			if (!content_length_str.empty())
			{
				content_length = std::atoll(content_length_str.data());
			}

			if (content_length > 0)
			{
				auto remain_size = static_cast<int64_t>(content_length - buffer.size());

				if (remain_size > 0)
				{
					ec = co_await session_ptr->async_read(buffer, remain_size);
				}
			}
			else
			{
				ec = asio::error::eof;
			}

			co_return ec;
		}

		template <bool Server>
		static auto parse_command_line(std::string_view header_span, error_code& ec)
		{
			using result_t =
				std::conditional_t<Server,
								   std::tuple<http_method, boost::system::result<boost::urls::url_view>, http_version>,
								   std::tuple<http_version, http_status>>;

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
				auto method = string_to_method(std::string_view(*iter++));

				if (!method.has_value())
				{
					return result_t{};
				}

				auto url_result = boost::urls::parse_origin_form(std::string_view(*iter++));
				if (url_result.has_error())
				{
					return result_t{};
				}

				auto version = string_to_version(std::string_view(*iter));

				if (!version.has_value())
				{
					return result_t{};
				}

				ec = error_code{};

				return std::make_tuple(*method, url_result, *version);
			}
			else
			{
				auto version = string_to_version(std::string_view(*iter++));

				auto status = static_cast<http_status>(std::atoi(std::string_view(*iter++).data()));

				ec = error_code{};

				return std::make_tuple(*version, status);
			}
		}

		static std::string_view parse_key(std::string_view header_str, const std::string& key)
		{
			std::string_view result{};

			auto pos = header_str.find(key);
			if (pos != std::string::npos)
			{
				auto end_pos = header_str.find_first_of("\r\n", pos);
				if (end_pos != std::string::npos)
				{
					auto start = header_str.find_first_of(':', pos);
					auto k = header_str.substr(pos, start - pos);
					if (k == key)
					{
						result = header_str.substr(start + 2, end_pos - start - 2);
					}
				}
			}

			return result;
		}
	};

	using http_session = basic_session<http, raw_adaptor>;
	using http_client = basic_client<http_session>;
	using http_server = basic_server<http_session>;

	using https_client_session = basic_session<http, ssl_adaptor<false>>;
	using https_server_session = basic_session<http, ssl_adaptor<true>>;
} // namespace aquarius