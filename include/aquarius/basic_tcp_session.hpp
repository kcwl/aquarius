#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/detail/config.hpp>

namespace aquarius
{
	template <bool Server, typename Flow, typename Header>
	class basic_tcp_session : public basic_session<Server>,
							  public std::enable_shared_from_this<basic_tcp_session<Server, Flow, Header>>
	{
		using base_type = basic_session<Server>;
		
	public:
		using base_type::is_server;

		using header = Header;

		using typename base_type::socket;

		using typename base_type::acceptor;

		using typename base_type::resolver;

	public:
		explicit basic_tcp_session(socket sock)
			: base_type(std::move(sock))
		{
		}

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
			constexpr auto header_length = header::impl_size;

			std::vector<char> header_buffer(header_length);
			ec = co_await this->async_read(header_buffer);
			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}
				this->shutdown();
				co_return;
			}
			header req_header{};
			req_header.consume(header_buffer);
			std::vector<char> body_buffer(req_header.length());
			ec = co_await this->async_read(body_buffer);
			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}
				this->shutdown();
				co_return;
			}
			Flow::recv(this->shared_from_this(), std::move(body_buffer), req_header);
		}
	};

} // namespace aquarius