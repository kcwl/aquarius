#pragma once
#include <aquarius/core/logger.hpp>
#include <boost/asio/basic_stream_socket.hpp>

namespace aquarius
{
	template <typename Protocol, typename Executor>
	class async_io_base
	{
	public:
		using socket_type = boost::asio::basic_stream_socket<Protocol, Executor>;

		struct implementation_type_base
		{
			socket_type* socket;
		};

	public:
		void construct(implementation_type_base& impl)
		{
			impl.socket = new socket_type(this->context());
		}

		void base_move_copy(implementation_type_base& impl, socket_type socket)
		{
			impl.socket = new socket_type(std::move(socket));
		}

		void base_destroy(implementation_type_base& impl)
		{
			if (!impl.socket->is_open())
				return;

			base_shutdown(impl);

			if (impl.socket)
				delete impl.socket;
		}

		void base_shutdown(implementation_type_base& impl)
		{
			if (!impl.socket->is_open())
				return;

			boost::system::error_code ec{};

			impl.socket->shutdown(boost::asio::socket_base::shutdown_both, ec);

			impl.socket->close(ec);
		}

		void close(implementation_type_base& impl, bool enable)
		{
			boost::system::error_code ec;

			if (!impl.socket->is_open())
				return;

			if (enable)
				return base_shutdown(impl);

			impl.socket->close(ec);
		}

		bool keep_alive(implementation_type_base& impl, bool value)
		{
			boost::system::error_code ec;

			impl.socket->set_option(boost::asio::socket_base::keep_alive(value), ec);

			XLOG_INFO() << "set keep alive :" << value;

			return !ec;
		}

		bool set_nodelay(implementation_type_base& impl, bool enable)
		{
			boost::system::error_code ec;
			impl.socket->set_option(boost::asio::ip::tcp::no_delay(enable), ec);

			XLOG_INFO() << "set nodelay :" << enable;

			return !ec;
		}

		std::string remote_address(const implementation_type_base& impl) const
		{
			return impl.socket->remote_endpoint().address().to_string();
		}

		uint32_t remote_address_u(const implementation_type_base& impl) const
		{
			return impl.socket->remote_endpoint().address().to_v4().to_uint();
		}

		uint16_t remote_port(const implementation_type_base& impl) const
		{
			return impl.socket->remote_endpoint().port();
		}
	};
} // namespace aquarius