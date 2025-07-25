#pragma once
#include <aquarius/error_code.hpp>
#include <aquarius/logger.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace aquarius
{
	namespace detail
	{
		namespace impl
		{
			class session_service_base
			{
			public:
				using protocol = boost::asio::ip::tcp;

				using socket = typename protocol::socket;

				using no_delay = typename protocol::no_delay;

				using acceptor = typename protocol::acceptor;

				using resolver = typename protocol::resolver;

				using endpoint = typename acceptor::endpoint_type;

				struct implementation_base
				{
					socket* socket_;
				};

			public:
				void move_copy(implementation_base& impl, socket sock)
				{
					impl.socket_ = new socket(std::move(sock));
				}

				void destroy(implementation_base& impl)
				{
					error_code ec{};

					impl.socket_->shutdown(boost::asio::socket_base::shutdown_both, ec);

					impl.socket_->close(ec);

					delete impl.socket_;
				}

				void shutdown(implementation_base& impl)
				{
					error_code ec{};

					impl.socket_->shutdown(boost::asio::socket_base::shutdown_both, ec);
				}

				void close(implementation_base& impl)
				{
					error_code ec{};

					impl.socket_->close(ec);
				}

				std::string remote_address(const implementation_base& impl) const
				{
					return impl.socket_->remote_endpoint().address().to_string();
				}

				uint32_t remote_address_u(const implementation_base& impl) const
				{
					return impl.socket_->remote_endpoint().address().to_v4().to_uint();
				}

				uint16_t remote_port(const implementation_base& impl) const
				{
					return impl.socket_->remote_endpoint().port();
				}

				bool keep_alive(implementation_base& impl, bool value)
				{
					error_code ec;

					impl.socket_->set_option(boost::asio::socket_base::keep_alive(value), ec);

					if (ec)
					{
						XLOG_ERROR() << "set keep alive failed! " << ec.what();
					}
					else
					{
						XLOG_INFO() << "set keep alive :" << value;
					}

					return !ec;
				}

				bool set_nodelay(implementation_base& impl, bool enable)
				{
					error_code ec;
					impl.socket_->set_option(typename boost::asio::ip::tcp::no_delay(enable), ec);

					if (ec)
					{
						XLOG_ERROR() << "set nodelay failed! " << ec.what();
					}
					else
					{
						XLOG_INFO() << "set nodelay :" << enable;
					}

					return !ec;
				}
			};
		} // namespace impl
	} // namespace detail
} // namespace aquarius