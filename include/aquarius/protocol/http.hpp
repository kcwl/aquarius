#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/detail/router.hpp>
#include <ranges>
#include <aquarius/virgo/http_header.hpp>
#include <aquarius/detail/flex_buffer.hpp>

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

					detail::router<Session>::get_mutable_instance().invoke(h.path(), session_ptr, buffer);
				},
				detached);
		}
	};
} // namespace aquarius