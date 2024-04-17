#pragma once
#include <aquarius/connect/socket_adapter.hpp>
#include <aquarius/context/session.hpp>
#include <aquarius/core/uuid.hpp>
#include <aquarius/router/router.hpp>

namespace aquarius
{
	template <typename _Protocol, conn_mode Conn, ssl_mode SSL>
	class connect : public std::enable_shared_from_this<connect<_Protocol, Conn, SSL>>
	{
		constexpr static ssl_mode SSLMode = SSL;

		constexpr static conn_mode ConnMode = Conn;

		using this_type = connect<_Protocol, ConnMode, SSLMode>;

		using socket_adapter_t = socket_adapter<ConnMode, SSLMode>;

	public:
		explicit connect(asio::socket_t socket, asio::ssl_context_t& ctx)
			: socket_(std::move(socket), ctx)
			, read_buffer_()
			, uuid_(uuid::invoke())
			, on_accept_()
			, on_close_()
		{}

		connect(asio::io_service& io_service, asio::ssl_context_t& basic_context)
			: connect(std::move(asio::ip::tcp::socket(io_service)), basic_context)
		{}

		virtual ~connect()
		{
			shut_down();
		}

		auto& socket()
		{
			return socket_.raw();
		}

		std::string remote_address()
		{
			return socket_.remote_address();
		}

		uint32_t remote_address_u()
		{
			return socket_.remote_address_u();
		}

		uint16_t remote_port()
		{
			return socket_.remote_port();
		}

		void start()
		{
			auto self = this->shared_from_this();

			return socket_.start([self, this] { this->establish_async_read(); });
		}

		void async_read()
		{
			read_buffer_.normalize();

			read_buffer_.ensure();

			auto self(this->shared_from_this());

			socket_.async_read_some(asio::buffer(read_buffer_.rdata(), read_buffer_.active()),
									[this, self](const asio::error_code& ec, std::size_t bytes_transferred)
									{
										if (ec)
										{
											if (ec != asio::error::eof)
											{
												XLOG_ERROR()
													<< "on read some at " << remote_address() << ":" << remote_port()
													<< "\t"
													   " occur error : "
													<< ec.message();
											}

											return shut_down();
										}

										read_buffer_.commit(static_cast<int>(bytes_transferred));

										message_router::process(read_buffer_, uuid());

										async_read();
									});
		}

		void async_write(flex_buffer_t&& resp_buf)
		{
			auto self(this->shared_from_this());

			socket_.async_write_some(
				asio::buffer(resp_buf.wdata(), resp_buf.size()),
				[this, self](const asio::error_code& ec, [[maybe_unused]] std::size_t bytes_transferred)
				{
					if (!ec)
					{
						return;
					}

					XLOG_ERROR() << "write error at " << remote_address() << "(" << remote_address_u() << "):"
								 << ":" << remote_port() << "\t" << ec.message();

					return shut_down();
				});
		}

		void shut_down()
		{
			on_close_(uuid());

			return socket_.shut_down();
		}

		void close(bool shutdown)
		{
			return socket_.close(shutdown);
		}

		std::size_t uuid() const
		{
			return uuid_;
		}

		void set_verify_mode(asio::ssl::verify_mode v)
		{
			socket_.set_verify_mode(v);
		}

		auto get_executor()
		{
			return socket_.get_executor();
		}

		template<typename _Func>
		void regist_accept(_Func&& f)
		{
			on_accept_ = std::forward<_Func>(f);
		}

		template<typename _Func>
		void regist_close(_Func&& f)
		{
			on_close_ = std::forward<_Func>(f);
		}

	private:
		void establish_async_read()
		{
			XLOG_INFO() << "handshake success at " << remote_address() << ":" << remote_port()
						<< ", async read establish";

			auto session_ptr = std::make_shared<session<this_type>>(this->shared_from_this());

			invoke_session_helper::push(session_ptr);

			on_accept_(uuid());

			keep_alive(true);

			if constexpr (ConnMode == conn_mode::basic_server)
			{
				reuse_address(true);

				set_nodelay(true);
			}

			async_read();
		}

		bool keep_alive(bool value) noexcept
		{
			asio::error_code ec;

			socket_.set_option(asio::socket_base::keep_alive(value), ec);

			XLOG_INFO() << "set keep alive :" << value;

			return !ec;
		}

		bool set_nodelay(bool enable)
		{
			asio::error_code ec;
			socket_.set_option(asio::ip::tcp::no_delay(enable), ec);

			XLOG_INFO() << "set nodelay :" << enable;

			return !ec;
		}

		bool reuse_address(bool value)
		{
			asio::error_code ec;
			socket_.set_option(asio::socket_base::reuse_address(value), ec);

			XLOG_INFO() << "reuse address :" << value;

			return !ec;
		}

	private:
		socket_adapter_t socket_;

		flex_buffer_t read_buffer_;

		std::size_t uuid_;

		std::function<void(const std::size_t)> on_accept_;

		std::function<void(const std::size_t)> on_close_;
	};
} // namespace aquarius
