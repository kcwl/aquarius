#pragma once
#include <aquarius/error_code.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/socket_base.hpp>

namespace aquarius
{
	namespace detail
	{
		template <typename Protocol>
		class session_service_base
		{
		public:
			using socket_type = typename Protocol::socket;

			struct implementation_type_base
			{
				socket_type* socket;
			};

		public:
			void move_copy(implementation_type_base& impl, socket_type socket)
			{
				impl.socket = new socket_type(std::move(socket));
			}

			void destroy(implementation_type_base& impl)
			{
				error_code ec{};

				impl.socket->shutdown(aquarius::socket_base::shutdown_both, ec);

				impl.socket->close(ec);

				delete impl.socket;
			}

			void shutdown(implementation_type_base& impl)
			{
				error_code ec{};

				impl.socket->shutdown(aquarius::socket_base::shutdown_both, ec);
			}

			void close(implementation_type_base& impl)
			{
				error_code ec{};

				impl.socket->close(ec);
			}

			std::string remote_address(implementation_type_base& impl) const
			{
				return impl.socket->remote_endpoint().address().to_string();
			}

			uint32_t remote_address_u(implementation_type_base& impl) const
			{
				return impl.socket->remote_endpoint().address().to_v4().to_uint();
			}

			uint16_t remote_port(implementation_type_base& impl) const
			{
				return impl.socket->remote_endpoint().port();
			}

			bool keep_alive(implementation_type_base& impl, bool value)
			{
				error_code ec;

				impl.socket->set_option(aquarius::socket_base::keep_alive(value), ec);

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

			bool set_nodelay(implementation_type_base& impl, bool enable)
			{
				error_code ec;
				impl.socket->set_option(typename Protocol::no_delay(enable), ec);

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
	} // namespace detail
} // namespace aquarius