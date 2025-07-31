#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/detail/config.hpp>
#include <aquarius/flow/raw_buffer_flow.hpp>
#include <aquarius/flow_context.hpp>

namespace aquarius
{
	template <bool Server, typename Header>
	class basic_tcp_session : public basic_session<Server>,
							  public std::enable_shared_from_this<basic_tcp_session<Server, Header>>
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
			header h{};
			h.consume(header_buffer);
			std::vector<char> body_buffer(h.length());
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

			std::size_t rpc_id = h.transfer() ? h.rpc() : rpc_transfer_flow::id;

			co_spawn(
				this->get_executor(),
				[buffer = std::move(body_buffer), session_ptr = this->shared_from_this(),
				 h = std::move(h), rpc_id]() mutable -> awaitable<void>
				{
					detail::handler_router<basic_tcp_session>::get_mutable_instance().invoke(rpc_id, session_ptr,
																							 std::move(buffer), h);
					co_return;
				},
				detached);
		}
	};

} // namespace aquarius