#pragma once
#include <aquarius/core/logger.hpp>
#include <aquarius/detail/io_concepts.hpp>
#include <aquarius/detail/socket_io_base.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/use_awaitable.hpp>

#define AQUARIUS_ENABLE_SSL

#ifdef AQUARIUS_ENABLE_SSL
#include <boost/asio/ssl.hpp>
#endif

namespace aquarius
{

	template <typename Protocol, typename Executor>
	class async_io_service
		: public boost::asio::detail::execution_context_service_base<async_io_service<Protocol, Executor>>,
		  public socket_io_base<Protocol, Executor>
	{
	public:
		using resolver_type = typename Protocol::resolver;

		using execution_base_type =
			boost::asio::detail::execution_context_service_base<async_io_service<Protocol, Executor>>;

		using io_base = socket_io_base<Protocol, Executor>;

		using executor_type = Executor;

		using typename io_base::socket_type;

		struct implementation_type : io_base::implementation_type_base
		{};

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
			impl.socket = new socket_type(this->context());
		}

		void move_copy(implementation_type& impl, socket_type socket)
		{
			this->base_move_copy(impl, std::move(socket));
		}

		void destroy(implementation_type& impl)
		{
			this->base_destroy(impl);
		}

		virtual void shutdown() override
		{
			return;
		}

		auto connect(implementation_type& impl, const std::string& ip_addr, const std::string& port,
					 boost::system::error_code& ec) -> boost::asio::awaitable<void>
		{
			resolver_type resolver(impl.socket->get_executor());

			auto endpoint = resolver.resolve(ip_addr, port);

			//co_await boost::asio::async_connect(impl.socket, endpoint, boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		}

		template <typename ConstBuffer>
		auto read(implementation_type& impl, ConstBuffer& buffer, std::size_t& bytes_transferred, boost::system::error_code& ec)
			-> boost::asio::awaitable<void>
		{
			bytes_transferred = co_await boost::asio::async_read(impl.socket, buffer, boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		}

		template <typename ConstBuffer>
		auto read_some(implementation_type& impl, ConstBuffer& buffer, std::size_t& bytes_transferred, boost::system::error_code& ec)
			-> boost::asio::awaitable<void>
		{
			bytes_transferred = co_await impl.socket->async_read_some(buffer, boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		}

		template <typename ConstBuffer>
		auto write(implementation_type& impl, const ConstBuffer& buffer, boost::system::error_code& ec)
			-> boost::asio::awaitable<void>
		{
			co_await impl.socket->async_write(buffer, boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		}

		template <typename ConstBuffer>
		auto write_some(implementation_type& impl, const ConstBuffer& buffer, boost::system::error_code& ec)
			-> boost::asio::awaitable<void>
		{
			co_await impl.socket->async_write_some(buffer, boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		}

		void start(implementation_type& impl)
		{
			XLOG_INFO() << "handshake success at " << io_base::remote_address(impl) << ":" << io_base::remote_port(impl)
						<< ", async read establish";

			this->keep_alive(impl, true);

			this->set_nodelay(impl, true);
		}
	};

#ifdef AQUARIUS_ENABLE_SSL
	template <bool IsServer, typename Protocol, typename Executor>
	class async_ssl_io_service : public boost::asio::detail::execution_context_service_base<
									 async_ssl_io_service<IsServer, Protocol, Executor>>,
								 public socket_io_base<Protocol, Executor>
	{
	public:
		using socket_type = boost::asio::basic_stream_socket<Protocol, Executor>;

		using io_base = socket_io_base<Protocol, Executor>;

		using resolver_type = typename Protocol::resolver;

		using ssl_socket_type = boost::asio::ssl::stream<socket_type&>;

		using ssl_context_type = boost::asio::ssl::context;

		using executor_type = Executor;

		using base_type =
			boost::asio::detail::execution_context_service_base<async_ssl_io_service<IsServer, Protocol, Executor>>;

		struct implementation_type : io_base::implementation_type_base
		{
			ssl_socket_type* ssl_socket;
		};

	public:
		explicit async_ssl_io_service(const Executor& ex)
			: base_type(ex)
		{}

		async_ssl_io_service(boost::asio::execution_context& context)
			: base_type(context)
		{}

	public:
		void construct(implementation_type& impl)
		{}

		void move_copy(implementation_type& impl, socket_type socket, ssl_context_type& ssl_context)
		{
			this->base_move_copy(impl, socket);

			impl.ssl_socket = new ssl_socket_type(*impl.socket, ssl_context);

			if constexpr (!IsServer)
			{
				impl.ssl_socket.set_verify_mode(boost::asio::ssl::verify_peer);
			}
		}

		void destroy(implementation_type& impl)
		{
			this->base_destroy(impl);

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

		auto connect(implementation_type& impl, const std::string& ip_addr, const std::string& port,
					 boost::system::error_code& ec) -> boost::asio::awaitable<void>
		{
			resolver_type resolve(impl.socket->get_executor());

			//auto endpoint = resolve.resolver(ip_addr, port);

			//co_await impl.socket->connect(endpoint,
			//										boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		}

		template <typename ConstBuffer>
		auto read(implementation_type& impl, ConstBuffer& buffer, std::size_t& bytes_transferred, boost::system::error_code& ec)
			-> boost::asio::awaitable<void>
		{
			bytes_transferred = co_await boost::asio::async_read(impl.ssl_socket, buffer, boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		}

		template <typename ConstBuffer>
		auto read_some(implementation_type& impl, ConstBuffer& buffer, std::size_t& bytes_transferred, boost::system::error_code& ec)
			-> boost::asio::awaitable<void>
		{
			bytes_transferred = co_await impl.ssl_socket->async_read_some(buffer,
													  boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		}

		template <typename ConstBuffer>
		auto write(implementation_type& impl, const ConstBuffer& buffer, boost::system::error_code& ec)
			-> boost::asio::awaitable<void>
		{
			co_await impl.ssl_socket->async_write(buffer, boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		}

		template <typename ConstBuffer>
		auto write_some(implementation_type& impl, const ConstBuffer& buffer, boost::system::error_code& ec)
			-> boost::asio::awaitable<void>
		{
			co_await impl.ssl_socket->async_write_some(buffer,
													   boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		}

		void start(implementation_type& impl)
		{
			XLOG_INFO() << "handshake success at " << io_base::remote_address(impl) << ":" << io_base::remote_port(impl)
						<< ", async read establish";

			boost::system::error_code ec{};
			handshake(impl, ec);

			if (!ec)
			{
				return;
			}

			keep_alive(impl, true);

			set_nodelay(impl, true);
		}

		void close(implementation_type& impl, bool enable)
		{
			boost::system::error_code ec;

			if (!impl.socket->is_open())
				return;

			if (enable)
				return this->base_shutdown(impl);

			impl.socket->close(ec);
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

		auto handshake(implementation_type& impl, boost::system::error_code& ec) -> boost::asio::awaitable<void>
		{
			co_await impl.ssl_socket->async_handshake(
				static_cast<boost::asio::ssl::stream_base::handshake_type>(IsServer),
				boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		}
	};
#endif
} // namespace aquarius