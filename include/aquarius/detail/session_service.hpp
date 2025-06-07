#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/protocol.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/detail/session_service_base.hpp>

namespace aquarius
{
	namespace detail
	{
		template <typename Protocol, typename Executor>
		class session_service
			: public boost::asio::detail::execution_context_service_base<session_service<Protocol, Executor>>,
			  public session_service_base<Protocol, Executor>
		{
		public:
			using base_type = session_service_base<Protocol, Executor>;

			using execution_base_type =
				boost::asio::detail::execution_context_service_base<session_service<Protocol, Executor>>;

			using socket_type = typename base_type::socket_type;

			using executor_type = Executor;

			struct implementation_type : base_type::implementation_type_base
			{};

		public:
			explicit session_service(const executor_type& ex)
				: execution_base_type(ex)
			{}

			session_service(boost::asio::execution_context& context)
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

			auto async_read_some(implementation_type& impl, error_code& ec) -> boost::asio::awaitable<flex_buffer>
			{
				flex_buffer buffer{};

				auto bytes_transferred =
					co_await impl.socket->async_read_some(boost::asio::buffer(buffer.rdata(), buffer.active()),
														  boost::asio::redirect_error(boost::asio::use_awaitable, ec));

				buffer.commit(bytes_transferred);

				co_return buffer;
			}

			auto async_write_some(implementation_type& impl, flex_buffer buffer, error_code& ec)
				-> boost::asio::awaitable<std::size_t>
			{
				co_return co_await impl.socket->async_write_some(
					boost::asio::buffer(buffer.wdata(), buffer.size()),
					boost::asio::redirect_error(boost::asio::use_awaitable, ec));
			}

			auto start(implementation_type& impl) -> boost::asio::awaitable<void>
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