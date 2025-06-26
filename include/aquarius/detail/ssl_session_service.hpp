#pragma once
#ifdef AQUARIUS_ENABLE_SSL
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/protocol.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/ssl/ssl_context_factory.hpp>


namespace aquarius
{
	namespace detail
	{
		template <bool Server, typename Protocol, typename Executor,
			std::size_t SSLVersion = boost::asio::ssl::context::sslv23>
		class ssl_session_service : public boost::asio::detail::execution_context_service_base<
			ssl_session_service<Server, Protocol, Executor, SSLVersion>>,
			public session_service_base<Protocol, Executor>
		{
		public:
			using base_type = session_service_base<Protocol, Executor>;

			using execution_base_type = boost::asio::detail::execution_context_service_base<
				ssl_session_service<Server, Protocol, Executor, SSLVersion>>;

			using executor_type = Executor;

			using socket_type = typename base_type::socket_type;

			using ssl_socket_type = boost::asio::ssl::stream<socket_type&>;

			struct implementation_type : base_type::implementation_type_base
			{
				ssl_socket_type* ssl_socket;
			};

		public:
			ssl_session_service(boost::asio::execution_context& context)
				: execution_base_type(context)
			{
			}

			virtual ~ssl_session_service() = default;

		public:
			void construct(implementation_type& /*impl*/)
			{
				return;
			}

			void move_copy(implementation_type& impl, socket_type socket)
			{
				base_type::move_copy(std::move(socket));

				impl.socket = new ssl_socket_type(*impl.socket, ssl_context_factory<Server, SSLVersion>::create());
			}

			void destroy(implementation_type& impl)
			{
				base_type::destroy(impl);

				error_code ec{};

				impl.ssl_socket->shutdown(ec);

				delete impl.ssl_socket;
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
				XLOG_INFO() << "start success at " << remote_address(impl) << ":" << remote_port(impl)
					<< ", async read establish";

				error_code ec;

				if constexpr (Server)
				{
					co_await impl.socket->async_handshake(boost::asio::ssl::stream_base::server,
						boost::asio::redirect_error(boost::asio::use_awaitable, ec));
				}
				else
				{
					co_await impl.socket->async_handshake(boost::asio::ssl::stream_base::client,
						boost::asio::redirect_error(boost::asio::use_awaitable, ec));
				}

				this->keep_alive(impl, true);

				this->set_nodelay(impl, true);
			}
		};
	}
} // namespace aquarius
#endif