#pragma once
#include <aquarius/detail/execution_context.hpp>
#include <aquarius/detail/session_service_base.hpp>
#include <aquarius/flex_buffer.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/redirect_error.hpp>
#include <aquarius/use_awaitable.hpp>
#include <boost/asio/read.hpp>

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

			using socket_type = typename Protocol::socket_type;

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

			template <typename BufferSequence>
			auto async_read(implementation_type& impl, BufferSequence& buff, error_code& ec) -> awaitable<void>
			{
				co_await boost::asio::async_read(*impl.socket, buffer(buff), redirect_error(use_awaitable, ec));
			}

			template <typename BufferSequence>
			auto async_write_some(implementation_type& impl, BufferSequence buff, error_code& ec)
				-> awaitable<std::size_t>
			{
				co_return co_await impl.socket->async_write_some(buffer(buff), redirect_error(use_awaitable, ec));
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