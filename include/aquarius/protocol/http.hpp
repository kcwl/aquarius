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
			co_await session_ptr->start();

			error_code ec;

			for (;;)
			{
				co_await recv(session_ptr, ec);

				if (ec)
				{
					if (ec != boost::asio::error::eof)
					{
						XLOG_ERROR() << "on read some occur error - " << ec.what();
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
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}
				session_ptr->shutdown();
				co_return;
			}

			auto [header_span, next_span] = parse_header_span(buffer);

			header h{};

			if (h.consume(header_span))
			{
				if (!h.path().empty())
				{
					co_spawn(
						session_ptr->get_executor(),
						[next_span, session_ptr = session_ptr->shared_from_this(),
						 h = std::move(h)]() mutable -> awaitable<void>
						{
							detail::router<Session>::get_mutable_instance().invoke(h.path(), session_ptr, next_span, h);
							co_return;
						},
						detached);
				}
			}
		}

		template <typename BufferSequence>
		auto parse_header_span(BufferSequence& buffer) -> std::pair<std::span<char>, std::span<char>>
		{
			constexpr std::string_view delm = "\r\n";

			constexpr auto delm_size = delm.size();

			auto buf_span = std::span<char>(buffer);

			auto buf_view = buf_span | std::views::slide(delm_size);

			auto itor = std::ranges::find_if(buf_view,
											 [](const auto& v)
											 {
												 if (v.size() < delm_size)
													 return false;

												 return std::string_view(v) == delm;
											 });

			if (itor == buf_view.end())
				return {};

			auto header_line_pos = std::distance(buf_view.begin(), itor);

			auto header_line_span = buf_span.subspan(0, header_line_pos);

			return { header_line_span, buf_span.subspan(header_line_pos + 2) };
		}
	};
} // namespace aquarius