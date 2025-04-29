#pragma once
#include <aquarius/core/logger.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <aquarius/detail/ssl_traits.hpp>

#define AQUARIUS_ENABLE_SSL

#ifdef AQUARIUS_ENABLE_SSL
#include <boost/asio/ssl.hpp>
#endif

namespace aquarius
{
	template <typename Protocol, typename Executor, bool IsServer = true, typename SSL = void>
	class async_io_service : public boost::asio::detail::execution_context_service_base<
								 async_io_service<Protocol, Executor, IsServer, SSL>>
	{
	public:
		constexpr static auto handshake = static_cast<boost::asio::ssl::stream_base::handshake_type>(IsServer);

		constexpr static std::size_t ssl_version = ssl_version_traits<SSL>::value;

		using resolver_type = typename Protocol::resolver;

		using execution_base_type =
			boost::asio::detail::execution_context_service_base<async_io_service<Protocol, Executor, IsServer, SSL>>;

		using executor_type = Executor;

		using ssl_context_type = std::conditional_t<std::is_same_v<SSL, void>, int,
#ifdef AQUARIUS_ENABLE_SSL
													boost::asio::ssl::context&
#else
													int
#endif
													>;
		using socket_type = boost::asio::basic_stream_socket<Protocol, Executor>;

		using ssl_socket_type = std::conditional_t<std::is_same_v<SSL, void>, null_ssl<socket_type, ssl_context_type>,
#ifdef AQUARIUS_ENABLE_SSL
												   boost::asio::ssl::stream<socket_type&>
#else
												   null_ssl<socket_type, ssl_context_type>
#endif
												   >;

		struct implementation_type
		{
			socket_type* socket;

			ssl_socket_type* ssl_socket;
		};

	public:
		explicit async_io_service(const executor_type& ex)
			: execution_base_type(ex)
		{}

		async_io_service(boost::asio::execution_context& context)
			: execution_base_type(context)
		{}

		virtual ~async_io_service() = default;

	public:
		void construct(implementation_type& impl)
		{
			// this->base_construct(impl);
		}

		void move_copy(implementation_type& impl, socket_type socket, ssl_context_type ssl_context)
		{
			impl.socket = new socket_type(std::move(socket));

			impl.ssl_socket = new ssl_socket_type(*impl.socket, ssl_context);
		}

		void destroy(implementation_type& impl)
		{
			if (!impl.socket->is_open())
				return;

			boost::system::error_code ec{};

			if (impl.socket)
			{
				impl.socket->shutdown(boost::asio::socket_base::shutdown_both, ec);

				impl.socket->close(ec);

				delete impl.socket;
			}

			if (impl.ssl_socket)
			{
				boost::system::error_code ec;

				impl.ssl_socket->shutdown(ec);

				delete impl.ssl_socket;
			}
		}

		virtual void shutdown() override
		{
			return;
		}

		auto async_connect(implementation_type& impl, const std::string& ip_addr, const std::string& port,
						   boost::system::error_code& ec) -> boost::asio::awaitable<void>
		{
			resolver_type resolver(impl.socket->get_executor());

			auto endpoint = resolver.resolve(ip_addr, port);

			co_await impl.socket->async_connect(endpoint, boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		}

		template <typename ConstBuffer>
		auto async_read_some(implementation_type& impl, ConstBuffer& buffer,
							 boost::system::error_code& ec) -> boost::asio::awaitable<std::size_t>
		{
			if constexpr (ssl_mode<SSL>)
			{
				co_return co_await impl.ssl_socket->async_read_some(
					buffer, boost::asio::redirect_error(boost::asio::use_awaitable, ec));
			}
			else
			{
				co_return co_await impl.socket->async_read_some(
					buffer, boost::asio::redirect_error(boost::asio::use_awaitable, ec));
			}
		}

		template <typename ConstBuffer>
		auto async_write_some(implementation_type& impl, const ConstBuffer& buffer, boost::system::error_code& ec)
			-> boost::asio::awaitable<std::size_t>
		{
			if constexpr (ssl_mode<SSL>)
			{
				co_return co_await impl.ssl_socket->async_write_some(buffer,
														   boost::asio::redirect_error(boost::asio::use_awaitable, ec));
			}
			else
			{
				co_return co_await impl.socket->async_write_some(buffer,
													   boost::asio::redirect_error(boost::asio::use_awaitable, ec));
			}
		}

		auto start(implementation_type& impl) -> boost::asio::awaitable<void>
		{
			XLOG_INFO() << "handshake success at " << remote_address(impl) << ":" << remote_port(impl)
						<< ", async read establish";

			if constexpr (ssl_mode<SSL>)
			{
				boost::system::error_code ec;

				co_await impl.ssl_socket->async_handshake(handshake,
														  boost::asio::redirect_error(boost::asio::use_awaitable, ec));
			}

			this->keep_alive(impl, true);

			this->set_nodelay(impl, true);

			co_return;
		}

		bool keep_alive(implementation_type& impl, bool value)
		{
			boost::system::error_code ec;

			impl.socket->set_option(boost::asio::socket_base::keep_alive(value), ec);

			XLOG_INFO() << "set keep alive :" << value;

			return !ec;
		}

		bool set_nodelay(implementation_type& impl, bool enable)
		{
			boost::system::error_code ec;
			impl.socket->set_option(boost::asio::ip::tcp::no_delay(enable), ec);

			XLOG_INFO() << "set nodelay :" << enable;

			return !ec;
		}

		std::string remote_address(const implementation_type& impl) const
		{
			return impl.socket->remote_endpoint().address().to_string();
		}

		uint32_t remote_address_u(const implementation_type& impl) const
		{
			return impl.socket->remote_endpoint().address().to_v4().to_uint();
		}

		uint16_t remote_port(const implementation_type& impl) const
		{
			return impl.socket->remote_endpoint().port();
		}
	};
} // namespace aquarius