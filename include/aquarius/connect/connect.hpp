#pragma once
#include <aquarius/connect/basic_connect.hpp>
#include <aquarius/connect/packet.hpp>
#include <aquarius/context/manager.hpp>
#include <aquarius/core/uuid.hpp>
#include <aquarius/core/core.hpp>

namespace aquarius
{
	template <typename _Protocol, typename _ServerType, template <typename> typename _Crypto>
	class connect : public basic_connect, public std::enable_shared_from_this<connect<_Protocol, _ServerType, _Crypto>>
	{
		constexpr static bool is_server = _ServerType::value;

		using this_type = connect<_Protocol, _ServerType, _Crypto>;

		using crypto_t = _Crypto<_ServerType>;

	public:
		explicit connect(asio::socket_t socket, asio::ssl_context_t& ctx)
			: socket_(std::move(socket))
			, crypto_(socket_, ctx)
			, read_buffer_(pack_limit)
			, uuid_(invoke_uuid<uint32_t>())
			, on_accept_()
			, on_close_()
			, receiver_(this)
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
			return socket_;
		}

		std::string remote_address()
		{
			return socket_.remote_endpoint().address().to_string();
		}

		uint32_t remote_address_u()
		{
			return socket_.remote_endpoint().address().to_v4().to_uint();
		}

		uint16_t remote_port()
		{
			return socket_.remote_endpoint().port();
		}

		void start()
		{
			auto self = this->shared_from_this();

			return crypto_.start([self, this] { this->establish_async_read(); });
		}

		void async_read()
		{
			read_buffer_.normalize();

			read_buffer_.ensure();

			auto self(this->shared_from_this());

			crypto_.async_read_some(asio::buffer(read_buffer_.rdata(), read_buffer_.active()),
									[this, self](const asio::error_code& ec, std::size_t bytes_transferred)
									{
										if (ec)
										{
											if (ec != asio::error::eof)
											{
												XLOG_ERROR() << "on read some occur error - " << ec.message();
											}

											return shut_down();
										}

										read_buffer_.commit(static_cast<int>(bytes_transferred));

										receiver_.process(read_buffer_);

										async_read();
									});
		}

		virtual void async_write(flex_buffer_t&& resp_buf) override
		{
			auto self(this->shared_from_this());

			crypto_.async_write_some(
				asio::buffer(resp_buf.wdata(), resp_buf.size()),
				[this, self](const asio::error_code& ec, [[maybe_unused]] std::size_t bytes_transferred)
				{
					if (!ec)
					{
						return;
					}
				});
		}

		void shut_down()
		{
			if (on_close_)
				on_close_(uuid_);

			return crypto_.shut_down();
		}

		void close(bool shutdown)
		{
			asio::error_code ec;

			if (!socket_.is_open())
				return;

			if (shutdown)
				return shut_down();

			socket_.close(ec);
		}

		virtual std::size_t uuid() const override
		{
			return uuid_;
		}

		template <typename _Func>
		void regist_accept(_Func&& f)
		{
			on_accept_ = std::forward<_Func>(f);
		}

		template <typename _Func>
		void regist_close(_Func&& f)
		{
			on_close_ = std::forward<_Func>(f);
		}

	private:
		void establish_async_read()
		{
			XLOG_INFO() << "handshake success at " << remote_address() << ":" << remote_port()
						<< ", async read establish";

			invoke_session_helper::push(this->shared_from_this());

			if (on_accept_)
				on_accept_(uuid());

			keep_alive(true);

			if constexpr (is_server)
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
		asio::socket_t socket_;

		crypto_t crypto_;

		flex_buffer_t read_buffer_;

		uint32_t uuid_;

		std::function<void(const std::size_t)> on_accept_;

		std::function<void(const std::size_t)> on_close_;

		packet<this_type> receiver_;
	};
} // namespace aquarius
