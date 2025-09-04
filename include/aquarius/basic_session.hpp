#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/concepts/concepts.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <span>
#include <aquarius/detail/ssl_context.hpp>
#include <boost/asio/read.hpp>

namespace aquarius
{

	template <bool Server, template <bool> typename Protocol, typename SSL = void>
	class basic_session : public std::enable_shared_from_this<basic_session<Server, Protocol, SSL>>
	{
		friend class Protocol<Server>;

	public:
		using protocol = Protocol<Server>;

		constexpr static auto is_server = Server;

		using socket = typename protocol::socket;

		using endpoint = typename protocol::endpoint;

		using acceptor = typename protocol::acceptor;

		using resolver = typename protocol::resolver;

		using header = typename protocol::header;

	public:
		explicit basic_session(socket sock)
			: socket_(std::move(sock))
			, uuid_(detail::uuid_generator()())
			, proto_()
		{}

		basic_session(io_context& io)
			: basic_session(socket(io))
		{}

		virtual ~basic_session() = default;

	public:
		auto get_executor()
		{
			return socket_.get_executor();
		}

		std::size_t uuid() const
		{
			return uuid_;
		}

		auto async_connect(const std::string& host, const std::string& port) -> awaitable<bool>
		{
			resolver resolve(socket_.get_executor());

			auto endpoints = resolve.resolve(host, port);

			error_code ec;

			co_await boost::asio::async_connect(socket_, endpoints, redirect_error(use_awaitable, ec));

			if (ec)
			{
				XLOG_ERROR() << "connect " << host << " failed! maybe" << ec.what();
			}

			co_return !ec;
		}

		std::string remote_address() const
		{
			return socket_.remote_endpoint().address().to_string();
		}

		uint32_t remote_address_u() const
		{
			return socket_.remote_endpoint().address().to_v4().to_uint();
		}

		uint16_t remote_port() const
		{
			return socket_.remote_endpoint().port();
		}

		template <typename T>
		auto async_read(detail::flex_buffer<T>& buffer) -> awaitable<error_code>
		{
			error_code ec;

			boost::asio::async_read(socket_, boost::asio::buffer(buffer.rdata(), buffer.active()), redirect_error(use_awaitable, ec));

			co_return ec;
		}

		template <typename T>
		auto async_read_some(detail::flex_buffer<T>& buffer) -> awaitable<error_code>
		{
			error_code ec;

			co_await socket_.async_read_some(boost::asio::buffer(buffer.rdata(), buffer.active()), redirect_error(use_awaitable, ec));

			co_return ec;
		}

		template <typename T>
		auto async_send(detail::flex_buffer<T> buffer) -> awaitable<void>
		{
			error_code ec{};

			co_await socket_.async_write_some(boost::asio::buffer(buffer.rdata(), buffer.active()), redirect_error(use_awaitable, ec));
		}

		void shutdown()
		{
			error_code ec;

			socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);
		}

		void close()
		{
			socket_.close();
		}

		auto accept() -> awaitable<error_code>
		{
			co_return co_await proto_.accept(this->shared_from_this());
		}

		bool keep_alive(bool value)
		{
			error_code ec;

			socket_.set_option(typename protocol::keep_alive(value), ec);

			return !ec;
		}

		bool set_nodelay(bool enable)
		{
			error_code ec;

			socket_.set_option(typename protocol::no_delay(enable), ec);

			return !ec;
		}

	protected:
		socket socket_;

		std::size_t uuid_;

		Protocol<Server> proto_;
	};

	template <bool Server, template <bool> typename Protocol>
	class basic_session<Server, Protocol, detail::ssl_context_factory<Server>>
		: public std::enable_shared_from_this<basic_session<Server, Protocol, detail::ssl_context_factory<Server>>>
	{
	public:
		using protocol = Protocol<Server>;

		constexpr static auto is_server = Server;

		using socket = typename protocol::socket;

		using endpoint = typename protocol::endpoint;

		using acceptor = typename protocol::acceptor;

		using resolver = typename protocol::resolver;

		using header = typename protocol::header;

		using ssl_socket = boost::asio::ssl::stream<socket>;

		using ssl_context = detail::ssl_context_factory<Server>;

	public:
		explicit basic_session(socket sock)
			: ssl_socket_(ssl_socket(std::move(sock), ssl_context::create()))
			, uuid_(detail::uuid_generator()())
			, proto_()
		{
			if constexpr (!Server)
			{
				ssl_socket_.set_verify_mode(boost::asio::ssl::verify_peer);

				ssl_socket_.set_verify_callback(
					[](bool verify, boost::asio::ssl::verify_context& ctx)
					{
						char subject_name[256];
						X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
						X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
						std::cout << "Verifying " << subject_name << "\n";

						return verify;
					});
			}
		}

		basic_session(io_context& io)
			: basic_session(socket(io))
		{}

		virtual ~basic_session() = default;

	public:
		auto get_executor()
		{
			return ssl_socket_.get_executor();
		}

		std::size_t uuid() const
		{
			return uuid_;
		}

		auto async_connect(const std::string& host, const std::string& port) -> awaitable<bool>
		{
			resolver resolve(ssl_socket_.lowest_layer().get_executor());

			auto endpoints = resolve.resolve(host, port);

			error_code ec;

			co_await boost::asio::async_connect(ssl_socket_.lowest_layer(), endpoints,
												redirect_error(use_awaitable, ec));

			if (ec)
			{
				XLOG_ERROR() << "connect " << host << " failed! maybe" << ec.what();
			}

			if (ec)
				co_return !ec;

			co_await ssl_socket_.async_handshake(boost::asio::ssl::stream_base::client,
												 redirect_error(use_awaitable, ec));

			if (ec)
			{
				XLOG_ERROR() << "connect " << host << " failed! maybe" << ec.what();
			}

			co_return !ec;
		}

		std::string remote_address() const
		{
			return ssl_socket_.lowest_layer().remote_endpoint().address().to_string();
		}

		uint32_t remote_address_u() const
		{
			return ssl_socket_.lowest_layer().remote_endpoint().address().to_v4().to_uint();
		}

		uint16_t remote_port() const
		{
			return ssl_socket_.lowest_layer().remote_endpoint().port();
		}

		template <typename T>
		auto async_read(detail::flex_buffer<T>& buffer, std::size_t length) -> awaitable<error_code>
		{
			error_code ec;

			length > buffer.remain() ? length = buffer.remain() : length;

			co_await boost::asio::async_read(ssl_socket_, boost::asio::buffer(buffer.rdata(), length),
											 redirect_error(use_awaitable, ec));

			co_return ec;
		}

		template <typename T>
		auto async_read_some(detail::flex_buffer<T>& buffer) -> awaitable<error_code>
		{
			error_code ec;

			co_await ssl_socket_.async_read_some(boost::asio::buffer(buffer.rdata(), buffer.active()), redirect_error(use_awaitable, ec));

			co_return ec;
		}

		template <typename T>
		auto async_send(detail::flex_buffer<T> buffer) -> awaitable<error_code>
		{
			error_code ec{};

			co_await ssl_socket_.async_write_some(boost::asio::buffer(buffer.rdata(), buffer.active()), redirect_error(use_awaitable, ec));

			co_return ec;
		}

		void shutdown()
		{
			error_code ec;

			ssl_socket_.shutdown(ec);
		}

		void close()
		{
			ssl_socket_.close();
		}

		auto accept() -> awaitable<error_code>
		{
			error_code ec;

			if constexpr (Server)
			{
				co_await ssl_socket_.async_handshake(boost::asio::ssl::stream_base::server,
													 redirect_error(use_awaitable, ec));

				if (ec)
					co_return ec;
			}

			co_return co_await proto_.accept(this->shared_from_this()); 
		}

		bool keep_alive(bool value)
		{
			error_code ec;

			ssl_socket_.lowest_layer().set_option(typename protocol::keep_alive(value), ec);

			return !ec;
		}

		bool set_nodelay(bool enable)
		{
			error_code ec;

			ssl_socket_.lowest_layer().set_option(typename protocol::no_delay(enable), ec);

			return !ec;
		}

	private:
		ssl_socket ssl_socket_;

		std::size_t uuid_;

		Protocol<Server> proto_;
	};
} // namespace aquarius