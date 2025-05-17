#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/ssl/ssl_context_factory.hpp>

namespace aquarius
{
	template <bool Server, typename Protocol, typename Executor, std::size_t SSLVersion>
	class basic_ssl_session_service : public boost::asio::detail::execution_context_service_base<
										  basic_ssl_session_service<Server, Protocol, Executor, SSLVersion>>
	{
	public:
		using endpoint_type = boost::asio::ip::basic_endpoint<Protocol>;

		using execution_base_type =
			boost::asio::detail::execution_context_service_base<basic_ssl_session_service<Server, Protocol, Executor, SSLVersion>>;

		using executor_type = Executor;

		using next_layer_type = boost::asio::basic_stream_socket<Protocol, Executor>;

		using socket_type = boost::asio::ssl::stream<next_layer_type>;

		struct implementation_type
		{
			socket_type* socket;
		};

	public:
		explicit basic_ssl_session_service(const executor_type& ex)
			: execution_base_type(ex)
		{}

		basic_ssl_session_service(boost::asio::execution_context& context)
			: execution_base_type(context)
		{}

		virtual ~basic_ssl_session_service() = default;

	public:
		void construct(implementation_type& impl)
		{
			if constexpr (Server)
			{
				impl.socket = new socket_type(std::move(socket_type(this->get_executor()), ssl_context_factory<SSLVersion>::create_server()));
			}
			else
			{
				impl.socket = new socket_type(std::move(socket_type(this->get_executor()), ssl_context_factory<SSLVersion>::create_client()));
			}
		}

		void move_copy(implementation_type& impl, socket_type socket)
		{
			if constexpr (Server)
			{
				*impl.socket = std::move(socket_type(std::move(socket), ssl_context_factory<SSLVersion>::create_server()));
			}
			else
			{
				*impl.socket = std::move(socket_type(std::move(socket), ssl_context_factory<SSLVersion>::create_client()));
			}
		}

		void destroy(implementation_type& impl)
		{
			if (!impl.socket) [[unlikely]]
				return;

			error_code ec{};

			impl.socket->shutdown(boost::asio::socket_base::shutdown_both, ec);

			impl.socket->close(ec);

			delete impl.socket;
		}

		virtual void shutdown() override
		{
			return;
		}

		error_code connect(implementation_type& impl, const std::string& ip_addr, const std::string& port,
						   error_code& ec)
		{
			auto resolver = boost::asio::ip::tcp::resolver(this->get_executor());

			auto endpoint = resolver.resolve(ip_addr, port);

			ec = boost::asio::connect(impl.socket->lowest_layer(), endpoint, ec);

			return ec;
		}

		auto async_connect(implementation_type& impl, const std::string& ip_addr, const std::string& port,
						   error_code& ec) -> boost::asio::awaitable<void>
		{
			auto resolver = boost::asio::ip::tcp::resolver(this->get_executor());

			auto endpoint = resolver.resolve(ip_addr, port);

			co_return co_await boost::asio::async_connect(impl.socket->lowest_layer(), endpoint,
														  boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		}

		error_code read_some(implementation_type& impl, flex_buffer_t& buffer, error_code& ec)
		{
			return impl.socket->read_some(boost::asio::buffer(buffer.rdata(), buffer.active()), ec);
		}

		auto async_read_some(implementation_type& impl, flex_buffer_t& buffer, error_code& ec)
			-> boost::asio::awaitable<std::size_t>
		{
			co_return co_await impl.socket->async_read_some(
				boost::asio::buffer(buffer.rdata(), buffer.active()),
				boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		}

		error_code write_some(implementation_type& impl, flex_buffer_t buffer, error_code& ec)
		{
			return impl.socket->write_some(boost::asio::buffer(buffer.wdata(), buffer.size()), ec);
		}

		auto async_write_some(implementation_type& impl, flex_buffer_t buffer, error_code& ec)
			-> boost::asio::awaitable<std::size_t>
		{
			co_return co_await impl.socket->async_write_some(
				boost::asio::buffer(buffer.wdata(), buffer.size()),
				boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		}

		auto start(implementation_type& impl) -> boost::asio::awaitable<void>
		{
			XLOG_INFO() << "start success at " << remote_address() << ":" << remote_port() << ", async read establish";

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

			co_return;
		}

		bool keep_alive(implementation_type& impl, bool value)
		{
			error_code ec;

			impl.socket->set_option(boost::asio::socket_base::keep_alive(value), ec);

			XLOG_INFO() << "set keep alive :" << value;

			return !ec;
		}

		bool set_nodelay(implementation_type& impl, bool enable)
		{
			error_code ec;
			impl.socket->set_option(boost::asio::ip::tcp::no_delay(enable), ec);

			XLOG_INFO() << "set nodelay :" << enable;

			return !ec;
		}

		std::string remote_address(implementation_type& impl) const
		{
			return impl.socket->lowest_layer().remote_endpoint().address().to_string();
		}

		uint32_t remote_address_u(implementation_type& impl) const
		{
			return impl.socket->lowest_layer().remote_endpoint().address().to_v4().to_uint();
		}

		uint16_t remote_port(implementation_type& impl) const
		{
			return impl.socket->lowest_layer().remote_endpoint().port();
		}
	};
} // namespace aquarius