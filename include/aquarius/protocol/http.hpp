#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/detail/router.hpp>
#include <ranges>
#include <aquarius/virgo/http_header.hpp>
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
		using header = virgo::http_header<Server>;

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
				[session_ptr, buffer = std::move(buffer)]() mutable -> awaitable<void>
				{
					header h{};

					auto result = h.consume(buffer);

					if (!result.has_value())
						co_return;

					if (h.method() == virgo::http_method::options)
					{
						virgo::http_options_protocol::request hop_req{};
						auto result = hop_req.consume(buffer);

						if(result.error().value() != static_cast<int>(virgo::http_status::ok))
							co_return;

						virgo::http_options_protocol::response hop_resp{};
						hop_resp.result(virgo::http_status::no_content);
						hop_resp.reason(virgo::get_http_status_string(virgo::http_status::no_content));
						hop_resp.version(hop_req.version());
						hop_resp.set_field("Access-Control-Allow-Origin", hop_req.find("Origin"));
						hop_resp.set_field("Access-Control-Allow-Methods", "POST,GET");
						hop_resp.set_field("Access-Control-Allow-Headers", hop_req.find("Access-Control-Request-Headers"));
						hop_resp.set_field("Access-Control-Max-Age", 86400);

						detail::flex_buffer<char> buffer{};
						result = hop_resp.commit(buffer);

						if (!result.has_value())
							co_return;

						co_await session_ptr->async_send(std::move(buffer));
					}
					else
					{
						detail::router<Session>::get_mutable_instance().invoke(h.path(), session_ptr, buffer);
					}
				},
				detached);
		}
	};
} // namespace aquarius