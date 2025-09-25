#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/http/http_router.hpp>
#include <ranges>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/virgo/impl/http_options.hpp>
#include <aquarius/serialize/parse_helper.hpp>

namespace aquarius
{
	template <bool Server>
	class http
	{
		constexpr static std::size_t max_http_length = 8192;

		using buffer_t = detail::flex_buffer<char>;

	public:
		template <typename Session>
		auto accept(std::shared_ptr<Session> session_ptr) -> awaitable<error_code>
		{
			error_code ec;

			for (;;)
			{
				co_await recv(session_ptr, ec);

				if (ec)
				{
					if (ec != boost::asio::error::eof)
					{
					}

					session_ptr->shutdown();

					break;
				}
			}

			co_return ec;
		}

	private:
		template <typename Session>
		auto recv(std::shared_ptr<Session> session_ptr, error_code& ec) -> awaitable<void>
		{
			buffer_t buffer{};

			ec = co_await session_ptr->async_read_some(buffer);

			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
				}
				session_ptr->shutdown();
				co_return;
			}

			co_spawn(
				session_ptr->get_executor(),
				[session_ptr, buffer = std::move(buffer), this]() mutable -> awaitable<void>
				{
					error_code ec{};
					virgo::http_method method;
					virgo::http_version version;
					std::string path;
					std::vector<std::pair<std::string, std::string>> querys;

					parse_header_line(buffer, method, version, path, querys, ec);
					if (ec.value() != static_cast<int>(virgo::http_status::ok))
						co_return;

					if (method == virgo::http_method::options)
					{
						virgo::http_options_protocol::request hop_req{};
						hop_req.consume(buffer, ec);

						if (ec.value() != static_cast<int>(virgo::http_status::ok))
							co_return;

						virgo::http_options_protocol::response hop_resp{};
						hop_resp.result(static_cast<int>(virgo::http_status::no_content));
						hop_resp.version(version);
 						hop_resp.set_field("Access-Control-Allow-Origin", hop_req.find("Origin"));
						hop_resp.set_field("Access-Control-Allow-Methods", "POST,GET");
						hop_resp.set_field("Access-Control-Allow-Headers",
										   hop_req.find("Access-Control-Request-Headers"));
						hop_resp.set_field("Access-Control-Max-Age", std::to_string(86400));

						detail::flex_buffer<char> buffer{};
						hop_resp.commit(buffer, ec);

						if (ec.value() != static_cast<int>(virgo::http_status::ok))
							co_return;

						co_await session_ptr->async_send(std::move(buffer));
					}
					else
					{
						http_router<Session>::get_mutable_instance().invoke(method, path, session_ptr, buffer);
					}
				},
				detached);
		}

		template <typename T>
		void parse_header_line(detail::flex_buffer<T>& buffer, virgo::http_method& method, virgo::http_version& version,
							   std::string& path, std::vector<std::pair<std::string, std::string>>& querys,
							   error_code& ec)
		{
			auto span = std::span<T>(buffer.rdata(), buffer.active());

			auto buf_view = span | std::views::slide(2);

			auto iter = std::ranges::find_if(buf_view,
											 [] (const auto& v)
											 {
												 if (v.size() < 2)
													 return false;

												 return std::string_view(v) == "\r\n";
											 });

			if (iter == buf_view.end())
			{
				ec = make_error_code(virgo::http_status::bad_request);
				return;
			}

			auto len = std::distance(buf_view.begin(), iter);

			buffer.consume(len + 2);

			auto header_line = span.subspan(0, len + 2);

			auto headers = header_line | std::views::split(' ');

			auto size = std::distance(headers.begin(), headers.end());

			if (size < 3)
			{
				ec = make_error_code(virgo::http_status::bad_request);
				return;
			}

			auto header_iter = headers.begin();

			parse_method(std::span<T>(*header_iter++), method, ec);
			if (ec.value() != static_cast<int>(virgo::http_status::ok))
				return;

			parse_uri(std::span<T>(*header_iter++), method, path, querys, ec);
			if (ec.value() != static_cast<int>(virgo::http_status::ok))
				return;

			parse_version(std::span<T>(*header_iter), version, ec);
		}

		template <typename T>
		void parse_method(std::span<T> buffer, virgo::http_method& method, error_code& ec)
		{
			ec = make_error_code(virgo::http_status::ok);

			std::string_view value(buffer.data(), buffer.size());

			if (value == "POST")
			{
				method = virgo::http_method::post;
			}
			else if (value == "GET")
			{
				method = virgo::http_method::get;
			}
			else if (value == "OPTIONS")
			{
				method = virgo::http_method::options;
			}
			else
			{
				ec = make_error_code(virgo::http_status::bad_request);
			}
		}

		template <typename T>
		void parse_uri(std::span<T> buffer, virgo::http_method method, std::string& path,
					   std::vector<std::pair<std::string, std::string>>& querys, error_code& ec)
		{
			ec = make_error_code(virgo::http_status::ok);

			if (buffer.empty())
			{
				ec = make_error_code(virgo::http_status::bad_request);
				return;
			}

			if (method != virgo::http_method::get)
			{
				path = std::string(buffer.data(), buffer.size());
				return;
			}

			auto iter = std::find(buffer.begin(), buffer.end(), '?');

			if (iter == buffer.end())
			{
				ec = make_error_code(virgo::http_status::bad_request);
				return;
			}

			auto pos = std::distance(buffer.begin(), iter);

			path = std::string(buffer.subspan(0, pos).data());

			parse_querys(buffer.subspan(pos + 1), querys, ec);
		}

		template <typename T>
		void parse_version(std::span<T> buffer, virgo::http_version& version, error_code& ec)
		{
			ec = make_error_code(virgo::http_status::ok);

			auto value = std::string_view(buffer.data());

			auto pos = value.find('.');
			if (pos == std::string::npos)
			{
				if (value == "HTTP2")
				{
					version = virgo::http_version::http2;
				}
				else if (value == "HTTP3")
				{
					version = virgo::http_version::http3;
				}
				else
				{
					ec = make_error_code(virgo::http_status::bad_request);
				}
			}
			else
			{
				auto suffix = value.substr(pos + 1);
				if (*suffix.data() == '1')
				{
					version = virgo::http_version::http1_1;
				}
				else if (*suffix.data() == '0')
				{
					version = virgo::http_version::http1_0;
				}
				else
				{
					ec = make_error_code(virgo::http_status::bad_request);
				}
			}

			
		}

		template <typename T>
		void parse_querys(std::span<T> buffer, std::vector<std::pair<std::string, std::string>>& querys, error_code& ec)
		{
			ec = make_error_code(virgo::http_status::ok);

			for (const auto& p : buffer | std::views::split('&'))
			{
				auto pos = std::string_view(p).find('=');
				if (pos == std::string_view::npos)
				{
					ec = make_error_code(virgo::http_status::bad_request);
					break;
				}

				querys.emplace_back();
				auto& query = querys.back();

				query.first = std::string(std::string_view(p).substr(0, pos));
				query.second = std::string(std::string_view(p).substr(pos + 1));
			}
		}
	};
} // namespace aquarius