#pragma once
#include <aquarius/detail/io_concepts.hpp>
#include <boost/asio/basic_socket.hpp>
#include <boost/asio/basic_socket_acceptor.hpp>

namespace aquarius
{
	template <typename Protocol, typename Executor>
	class sync_io_acceptor
	{
		using impl_type = boost::asio::basic_socket_acceptor<Protocol, Executor>;

		using typename impl_type::executor_type;

		using typename impl_type::protocol_type;

		using typename impl_type::endpoint_type;

	public:
		sync_io_acceptor(const executor_type& ex, uint16_t port, bool reuse_addr = true)
			: impl_(ex, endpoint_type{ protocol_type::v4(), port }, reuse_addr)
		{}

		template <execution_context_convertible ExecutionContext>
		sync_io_acceptor(ExecutionContext& context, uint16_t port, bool reuse_addr = true)
			: impl_(context, endpoint_type{ protocol_type::v4(), port }, reuse_addr)
		{}

		sync_io_acceptor(sync_io_acceptor&&) = default;

		sync_io_acceptor& operator=(sync_io_acceptor&&) = default;

	public:
		auto accept() -> std::pair<typename protocol_type::socket, boost::system::error_code>
		{
			boost::system::error_code ec;

			auto sock = impl_.accept(ec);

			return { sock, ec };
		}

	private:
		impl_type impl_;
	};

	template <typename Protocol, typename Executor>
	class sync_io_service
	{
		using socket_type = boost::asio::basic_socket<Protocol, Executor>;

		using resolver_type = typename Protocol::resolver;

	public:
		explicit sync_io_service(const Executor& ex)
			: socket_(ex)
		{}

		template <execution_context_convertible ExecutionContext>
		explicit sync_io_service(ExecutionContext& context)
			: socket_(context)
		{}

		sync_io_service(socket_type socket)
			: socket_(std::move(socket))
		{}

	public:
		void connect(const std::string& ip_addr, const std::string& port, boost::system::error_code& ec)
		{
			resolver_type resolve(socket_.get_executor());

			auto endpoint = resolve.resolver(ip_addr, port);

			socket_.connect(endpoint, ec);
		}

		template <typename ConstBuffer>
		auto read(ConstBuffer& buffer, boost::system::error_code& ec)
		{
			return socket_.template read(buffer, ec);
		}

		template <typename ConstBuffer>
		auto read_some(ConstBuffer& buffer, boost::system::error_code& ec)
		{
			return socket_.template read_some(buffer, ec);
		}

		template <typename ConstBuffer>
		void write(const ConstBuffer& buffer, boost::system::error_code& ec)
		{
			return socket_.template write(buffer, ec);
		}

		template <typename ConstBuffer>
		auto write_some(const ConstBuffer& buffer, boost::system::error_code& ec)
		{
			socket_.template write_some(buffer, ec);
		}

	private:
		socket_type& socket_;
	};

#ifdef AQUARIUS_ENABLE_SSL
	template <typename Protocol, typename Executor>
	class sync_ssl_io_service
		: public boost::asio::detail::execution_context_service_base<sync_ssl_io_service<Protocol, Executor>>
	{
		using socket_type = boost::asio::basic_socket<Protocol, Executor>;

		using resolver_type = typename Protocol::resolver;

		using ssl_socket_t = boost::asio::ssl::stream<socket_type&>;

		using ssl_context_t = boost::asio::ssl::context;

	public:
		explicit sync_ssl_io_service(const Executor& ex)
			: socket_(ex)
			, ssl_context_()
			, ssl_socket_(socket_, ssl_context_)
		{}

		template <execution_context_convertible ExecutionContext>
		explicit sync_ssl_io_service(ExecutionContext& context)
			: socket_(context)
			, ssl_context_()
			, ssl_socket_(socket_, ssl_context_)
		{}

		sync_ssl_io_service(socket_type socket)
			: socket_(std::move(socket))
			, ssl_context_()
			, ssl_socket_(socket_, ssl_context_)
		{}

		void connect(const std::string& ip_addr, const std::string& port, boost::system::error_code& ec)
		{
			resolver_type resolve(socket_.get_executor());

			auto endpoint = resolve.resolver(ip_addr, port);

			ssl_socket_.connect(endpoint, ec);
		}

		template <typename ConstBuffer>
		void read(ConstBuffer& buffer, boost::system::error_code& ec)
		{
			ssl_socket_.template read(buffer, ec);
		}

		template <typename ConstBuffer>
		void read_some(ConstBuffer& buffer, boost::system::error_code& ec)
		{
			co_await ssl_socket_.template read_some(buffer, ec);
		}

		template <typename ConstBuffer>
		void write(const ConstBuffer& buffer, boost::system::error_code& ec)
		{
			co_await ssl_socket_.template write(buffer, ec);
		}

		template <typename ConstBuffer>
		void write_some(const ConstBuffer& buffer, boost::system::error_code& ec)
		{
			co_await ssl_socket_.template write_some(buffer, ec);
		}

	private:
		socket_type& socket_;

		asio::ssl_context_t& ssl_context_;

		asio::ssl_socket_t ssl_socket_;
	};
#endif

} // namespace aquarius