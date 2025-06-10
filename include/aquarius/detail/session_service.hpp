#pragma once
#include <aquarius/buffer.hpp>
#include <aquarius/detail/execution_context.hpp>
#include <aquarius/detail/session_service_base.hpp>
#include <aquarius/flex_buffer.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/redirect_error.hpp>
#include <aquarius/use_awaitable.hpp>

namespace aquarius
{
	namespace detail
	{
		template <typename Protocol>
		class session_service : public execution_context_service_base<session_service<Protocol>>,
								public session_service_base<Protocol>
		{
		public:
			using base_type = session_service_base<Protocol>;

			using execution_base_type = execution_context_service_base<session_service<Protocol>>;

			using socket_type = typename Protocol::socket;

			struct implementation_type : base_type::implementation_type_base
			{};

		public:
			session_service(execution_context& context)
				: execution_base_type(context)
			{}

			virtual ~session_service() = default;

		public:
			void construct(implementation_type& /*impl*/)
			{
				return;
			}

			void move_copy(implementation_type& impl, socket_type socket)
			{
				return base_type::move_copy(impl, std::move(socket));
			}

			void destroy(implementation_type& impl)
			{
				return base_type::destroy(impl);
			}

			virtual void shutdown() override
			{
				return;
			}

			auto async_read_some(implementation_type& impl, error_code& ec) -> awaitable<flex_buffer>
			{
				flex_buffer buff{};

				auto bytes_transferred = co_await impl.socket->async_read_some(buffer(buff.rdata(), buff.active()),
																			   redirect_error(use_awaitable, ec));

				buff.commit(bytes_transferred);

				co_return buff;
			}

			auto async_write_some(implementation_type& impl, flex_buffer buf, error_code& ec) -> awaitable<std::size_t>
			{
				co_return co_await impl.socket->async_write_some(buffer(buf.wdata(), buf.size()),
																 redirect_error(use_awaitable, ec));
			}

			auto start(implementation_type& impl) -> awaitable<void>
			{
				XLOG_INFO() << "start success at " << this->remote_address(impl) << ":" << this->remote_port(impl)
							<< ", async read establish";

				this->keep_alive(impl, true);

				this->set_nodelay(impl, true);

				co_return;
			}
		};
	} // namespace detail
} // namespace aquarius