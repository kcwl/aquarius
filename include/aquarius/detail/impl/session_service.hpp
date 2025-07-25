#pragma once
#include <aquarius/detail/execution_context.hpp>
#include <aquarius/detail/redirect_error.hpp>
#include <aquarius/detail/impl/session_service_base.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/use_awaitable.hpp>
#include <boost/asio/read.hpp>

namespace aquarius
{
	namespace detail
	{
		namespace impl
		{
			class session_service : public aquarius::detail::execution_context_service_base<session_service>,
									public session_service_base
			{
			public:
				using base = session_service_base;

				using typename base::protocol;

				using execution_base = aquarius::detail::execution_context_service_base<session_service>;

				using typename base::socket;

				using typename base::endpoint;

				struct implementation_type : base::implementation_base
				{};

			public:
				session_service(aquarius::detail::execution_context& context)
					: execution_base(context)
				{}

				virtual ~session_service() = default;

			public:
				void construct(implementation_type& /*impl*/)
				{
					return;
				}

				void move_copy(implementation_type& impl, socket sock)
				{
					return base::move_copy(impl, std::move(sock));
				}

				void destroy(implementation_type& impl)
				{
					return base::destroy(impl);
				}

				virtual void shutdown() override
				{
					return;
				}

				template <typename BufferSequence>
				auto async_read(implementation_type& impl, BufferSequence& buff, error_code& ec) -> awaitable<void>
				{
					co_await boost::asio::async_read(*impl.socket_, buffer(buff), redirect_error(use_awaitable, ec));
				}

				template <typename BufferSequence>
				auto async_write_some(implementation_type& impl, BufferSequence buff, error_code& ec)
					-> awaitable<std::size_t>
				{
					co_return co_await impl.socket_->async_write_some(buffer(buff), redirect_error(use_awaitable, ec));
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
		} // namespace impl
	} // namespace detail
} // namespace aquarius