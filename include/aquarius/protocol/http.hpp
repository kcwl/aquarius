#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/detail/router.hpp>
#include <ranges>
#include <virgo.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace aquarius
{
	template <bool Server>
	class http
	{
	public:
		constexpr static std::size_t max_http_length = 8192;

		using socket = boost::asio::ip::tcp::socket;

		using endpoint = boost::asio::ip::tcp::endpoint;

		using acceptor = boost::asio::ip::tcp::acceptor;

		using resolver = boost::asio::ip::tcp::resolver;

		using no_delay = boost::asio::ip::tcp::no_delay;

		using keep_alive = boost::asio::socket_base::keep_alive;

		using header = virgo::http::detail::basic_header<Server>;

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
						XLOG_ERROR() << "on read some occur error - " << ec.what();
					}

					session_ptr->shutdown();

					co_return ec;
				}
			}

			std::unreachable();
		}

	public:
		static endpoint make_v4_endpoint(uint16_t port)
		{
			return endpoint(boost::asio::ip::tcp::v4(), port);
		}

		static endpoint make_v6_endpoint(uint16_t port)
		{
			return endpoint(boost::asio::ip::tcp::v6(), port);
		}

	private:
		template <typename Session>
		auto recv(std::shared_ptr<Session> session_ptr, error_code& ec) -> awaitable<void>
		{
			std::vector<char> buffer(max_http_length);

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