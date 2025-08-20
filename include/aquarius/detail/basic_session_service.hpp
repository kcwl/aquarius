#pragma once
#include <aquarius/detail/execution_context.hpp>
#include <aquarius/detail/redirect_error.hpp>
#include <aquarius/detail/session_service_base.hpp>
#include <aquarius/flex_buffer.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/use_awaitable.hpp>
#include <boost/asio/read.hpp>

namespace aquarius
{
	namespace detail
	{

		template <typename Protocol>
		class basic_session_service : public execution_context_service_base<basic_session_service<Protocol>>
		{
		public:
			using execution_base = execution_context_service_base<basic_session_service>;

			using protocol = boost::asio::ip::tcp;

			using socket = typename protocol::socket;

			using no_delay = typename protocol::no_delay;

			using acceptor = typename protocol::acceptor;

			using resolver = typename protocol::resolver;

			using endpoint = typename acceptor::endpoint_type;

			struct implementation_type
			{
				socket* socket_;
			};

		public:
			basic_session_service(execution_context& context)
				: execution_base(context)
			{}

			virtual ~basic_session_service() = default;

		public:
			void move_copy(implementation_type& impl, socket sock)
			{
				impl.socket_ = new socket(std::move(sock));
			}

			void destroy(implementation_type& impl)
			{
				error_code ec{};

				impl.socket_->shutdown(boost::asio::socket_base::shutdown_both, ec);

				impl.socket_->close(ec);

				delete impl.socket_;
			}

			void shutdown(implementation_type& impl)
			{
				error_code ec{};

				impl.socket_->shutdown(boost::asio::socket_base::shutdown_both, ec);
			}

			void close(implementation_type& impl)
			{
				error_code ec{};

				impl.socket_->close(ec);
			}

			std::string remote_address(const implementation_type& impl) const
			{
				return impl.socket_->remote_endpoint().address().to_string();
			}

			uint32_t remote_address_u(const implementation_type& impl) const
			{
				return impl.socket_->remote_endpoint().address().to_v4().to_uint();
			}

			uint16_t remote_port(const implementation_type& impl) const
			{
				return impl.socket_->remote_endpoint().port();
			}

			bool keep_alive(implementation_type& impl, bool value)
			{
				error_code ec;

				impl.socket_->set_option(boost::asio::socket_base::keep_alive(value), ec);

				return !ec;
			}

			bool set_nodelay(implementation_type& impl, bool enable)
			{
				error_code ec;
				impl.socket_->set_option(typename boost::asio::ip::tcp::no_delay(enable), ec);

				return !ec;
			}

			void construct(implementation_type& /*impl*/)
			{
				return;
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
			auto async_read_some(implementation_type& impl, BufferSequence& buff, error_code& ec) -> awaitable<void>
			{
				co_await impl.socket_->async_read_some(buffer(buff), redirect_error(use_awaitable, ec));
			}

			template <typename BufferSequence>
			auto async_write_some(implementation_type& impl, BufferSequence buff, error_code& ec)
				-> awaitable<std::size_t>
			{
				co_return co_await impl.socket_->async_write_some((buffer)(buff), redirect_error(use_awaitable, ec));
			}

			auto start(implementation_type& impl) -> awaitable<void>
			{
				this->keep_alive(impl, true);

				this->set_nodelay(impl, true);

				co_return;
			}
		};
	} // namespace detail
} // namespace aquarius