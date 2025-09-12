#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/detail/router.hpp>
#include <ranges>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/virgo/impl/http_options.hpp>

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

					auto method = parse_method(buffer);

					if (!method.has_value())
						co_return;

					auto router = parse_uri(buffer);

					if (!router.has_value())
						co_return;

					auto version = parse_version(buffer);

					if (!version.has_value())
						co_return;

					auto version = result.value();

					if (method.value() == virgo::http_method::options)
					{
						virgo::http_options_protocol::request hop_req{};
						auto result = hop_req.consume(buffer);

						if (result.error().value() != static_cast<int>(virgo::http_status::ok))
							co_return;

						virgo::http_options_protocol::response hop_resp{};
						hop_resp.result(virgo::http_status::no_content);
						hop_resp.reason(virgo::from_status_string(virgo::http_status::no_content));
						hop_resp.version(hop_req.version());
						hop_resp.set_field("Access-Control-Allow-Origin", hop_req.find("Origin"));
						hop_resp.set_field("Access-Control-Allow-Methods", "POST,GET");
						hop_resp.set_field("Access-Control-Allow-Headers",
										   hop_req.find("Access-Control-Request-Headers"));
						hop_resp.set_field("Access-Control-Max-Age", 86400);

						detail::flex_buffer<char> buffer{};
						result = hop_resp.commit(buffer);

						if (!result.has_value())
							co_return;

						co_await session_ptr->async_send(std::move(buffer));
					}
					else
					{
						detail::router<Session>::get_mutable_instance().invoke(router, session_ptr, buffer);
					}
				},
				detached);
		}

		template <typename T>
		std::expected<virgo::http_method, error_code> parse_method(detail::flex_buffer<T>& buffer)
		{
			return read_value<T, ' '>(buffer).and_then(
				[&](const auto& value) -> std::expected<virgo::http_method, error_code>
				{
					virgo::http_method method{};

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
						return std::unexpected(make_error_code(virgo::http_status::bad_request));
					}

					return method;
				});
		}

		template <typename T>
		std::expected<std::string, error_code> parse_uri(detail::flex_buffer<T>& buffer)
		{
			return parse_path(buffer).and_then(
				[&](char end) -> std::expected<bool, error_code>
				{
					if (end == ' ')
						return true;

					querys_.push_back({});
					return parse_querys(buffer, querys_.back())
						.and_then(
							[&](char end) -> std::expected<bool, error_code>
							{
								if (end == '#' || end == ' ')
									return true;

								return std::unexpected(make_error_code(http_status::bad_request));
							});
				});
		}

		template <typename T>
		std::expected<virgo::http_version, error_code> parse_version(detail::flex_buffer<T>& buffer)
		{
			return read_value<T, '\r'>(buffer).and_then(
				[&](const auto& value) -> std::expected<bool, error_code>
				{
					auto pos = value.find('.');
					if (pos == std::string::npos)
					{
						if (value == "HTTP2")
						{
							version_ = http_version::http2;
						}
						else if (value == "HTTP3")
						{
							version_ = http_version::http3;
						}
						else
						{
							return std::unexpected(make_error_code(http_status::bad_request));
						}
					}
					else
					{
						auto suffix = value.substr(pos + 1);
						if (*suffix.data() == '1')
						{
							version_ = http_version::http1_1;
						}
						else
						{
							version_ = http_version::http1_0;
						}
					}

					buffer.consume(1);

					return true;
				});
		}

		template <typename T>
		std::expected<char, error_code> parse_path(detail::flex_buffer<T>& buffer)
		{
			auto c = buffer.peek();

			if (c != '/')
				return std::unexpected(make_error_code(http_status::bad_request));

			path_.push_back(buffer.get());

			while (buffer.peek() != T(-1))
			{
				c = buffer.get();

				if (c == '?' || c == '#' || c == ' ')
					break;

				if (!std::isalnum(c) && (c != '_'))
					return std::unexpected(make_error_code(http_status::bad_request));

				path_.push_back(c);
			}

			return c;
		}

		template <typename T>
		std::expected<char, error_code> parse_querys(detail::flex_buffer<T>& buffer,
													 std::pair<std::string, std::string>& query)
		{
			std::expected<char, error_code> result{};

			while (buffer.peek() != T(-1))
			{
				result = read_value<T, '='>(buffer)
							 .and_then(
								 [&](const auto& value)
								 {
									 query.first = value;

									 return read_value<T, '&', ' '>(buffer);
								 })
							 .and_then(
								 [&](const auto& value) -> std::expected<bool, error_code>
								 {
									 query.second = value;
									 return true;
								 });

				if (!result.has_value())
					break;

				if (result.value() == ' ')
					break;
			}

			return result;
		}
	};
} // namespace aquarius