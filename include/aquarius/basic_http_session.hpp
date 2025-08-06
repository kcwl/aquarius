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
		using base_type::is_server;

		using http_request_header = basic_http_header<true>;

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
			std::array<char, max_http_length> buffer{};

			co_await this->service().async_read_some(buffer, ec);

			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}
				this->shutdown();
				co_return;
			}

			http_request_header header{};

			header.consume(buffer);

			if (!header.path().empty())
			{
				//find req
				co_spawn(
					this->get_executor(),
					[buffer = std::move(body_buffer), session_ptr = this->shared_from_this(), h = std::move(header)]() mutable -> awaitable<void>
					{
						detail::handler_router<basic_http_session>::get_mutable_instance().invoke(header.path(), session_ptr,
							std::move(buffer), h);
						co_return;
					},
					detached);
			}
		}
	};
} // namespace aquarius