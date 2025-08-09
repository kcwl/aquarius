#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius_protocol.hpp>

namespace aquarius
{
	template <bool Server>
	class basic_http_session : public basic_session<Server>,
							   public std::enable_shared_from_this<basic_http_session<Server>>
	{
		using base_type = basic_session<Server>;

		constexpr static std::size_t max_http_length = 8192;
	public:
		using header = basic_http_header<Server>;

	public:
		using base_type::is_server;

		using typename base_type::socket;

		using typename base_type::acceptor;

		using typename base_type::resolver;

	public:
		explicit basic_http_session(socket sock)
			: base_type(std::move(sock))
		{}

	public:
		auto protocol() -> awaitable<error_code>
		{
			co_await this->service().start(this->implementation());

			error_code ec;

			for (;;)
			{
				co_await recv(ec);

				if (ec)
				{
					if (ec != boost::asio::error::eof)
					{
						XLOG_ERROR() << "on read some occur error - " << ec.what();
					}

					this->shutdown();

					co_return ec;
				}
			}

			std::unreachable();
		}

	private:
		auto recv(error_code& ec) -> awaitable<void>
		{
			std::vector<char> buffer(max_http_length);

			co_await this->service().async_read_some(this->implementation(), buffer, ec);

			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}
				this->shutdown();
				co_return;
			}

			auto header_span = parse_header_span(buffer);

			header h{};

			if (h.consume(header_span))
			{
				if (!h.path().empty())
				{
					co_spawn(
						this->get_executor(),
						[buffer = std::move(buffer), session_ptr = this->shared_from_this(), h = std::move(h)]() mutable -> awaitable<void>
						{
							detail::handler_router<basic_http_session>::get_mutable_instance().invoke(h.path(), session_ptr,
								std::move(buffer), h);
							co_return;
						},
						detached);
				}
			}
		}

		template<typename BufferSequence>
		auto parse_header_span(BufferSequence& buffer)
		{
			constexpr std::string_view delm = "\r\n";

			constexpr auto delm_size = delm.size();

			auto buf_span = std::span<char>(buffer);

			auto buf_view = buf_span | std::views::slide(delm_size);

			auto itor = std::ranges::find_if(buf_view, [](const auto& v)
				{
					if (v.size() < delm_size)
						return false;

					return std::string_view(v) == delm;
				});

			if (itor == buf_view.end())
				return;

			auto header_line_pos = std::distance(buf_view.begin(), itor);
			auto header_line_span = buf_span.subspan(0, header_line_pos);

			auto dy_buffer = boost::asio::dynamic_buffer(buffer);

			dy_buffer.consume(header_line_span.size());

			return header_line_span;
		}
	};
} // namespace aquarius