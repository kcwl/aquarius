#pragma once
#include <aquarius/concepts/concepts.hpp>
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <span>
#include <aquarius/detail/ssl_context.hpp>
#include <boost/asio/read.hpp>
#include <aquarius/coroutine.hpp>
#include <boost/asio/connect.hpp>
#include <aquarius/detail/redirect_error.hpp>
#include <aquarius/adaptor/sql_adaptor.hpp>
#include <aquarius/adaptor/timer_adaptor.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <aquarius/ip/tcp/tcp_error.hpp>
#include <aquarius/ip/concept.hpp>

namespace aquarius
{
	template <bool Server, typename Protocol>
	class basic_session : public std::enable_shared_from_this<basic_session<Server, Protocol>>
	{
	public:
		using protocol = Protocol;

		constexpr static auto is_server = Server;

		using socket = protocol::socket;

		using endpoint = protocol::endpoint;

		using acceptor = protocol::acceptor;

		using resolver = protocol::resolver;

		using ssl_socket = boost::asio::ssl::stream<socket>;

		using ssl_context = detail::ssl_context_factory<Server>;

	public:
		explicit basic_session(socket sock)
			: ssl_socket_(ssl_socket(std::move(sock), ssl_context::create()))
			, uuid_(detail::uuid_generator()())
			, proto_()
			, sql_(ssl_socket_.get_executor())
			, timer_(ssl_socket_.get_executor())
		{
			ssl_context::init(ssl_socket_);
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

		auto async_connect(const std::string& host, const std::string& port) -> awaitable<error_code>
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
				co_return ec;

			co_await ssl_socket_.async_handshake(boost::asio::ssl::stream_base::client,
												 redirect_error(use_awaitable, ec));

			if (ec)
			{
				XLOG_ERROR() << "connect " << host << " failed! maybe" << ec.what();
			}

			co_return ec;
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
		auto async_read(flex_buffer<T>& buffer, std::size_t length) -> awaitable<error_code>
		{
			error_code ec;

			if (length > buffer.remain())
			{
				co_return ec = tcp_error::no_space;
			}

			co_return co_await async_read(buffer.wdata(), length);
		}

		auto async_read(char* begin, std::size_t length) -> awaitable<error_code>
		{
			error_code ec;

			co_await boost::asio::async_read(ssl_socket_, boost::asio::buffer(begin, length),
											 redirect_error(use_awaitable, ec));

			co_return ec;
		}

		template <typename T>
		auto async_read_util(flex_buffer<T>& buffer, std::string_view delm) -> awaitable<error_code>
		{
			error_code ec;

			boost::asio::streambuf buf{};

			co_await boost::asio::async_read_until(ssl_socket_, buf, delm, redirect_error(use_awaitable, ec));

			if (!ec)
			{
				buffer.put((char*)buf.data().data(), buf.size());
			}

			co_return ec;
		}

		template <typename T>
		auto async_send(flex_buffer<T> buffer) -> awaitable<error_code>
		{
			error_code ec{};

			co_await ssl_socket_.async_write_some(boost::asio::buffer(buffer.rdata(), buffer.active()),
												  redirect_error(use_awaitable, ec));

			co_return ec;
		}

		void shutdown()
		{
			error_code ec;

			ssl_socket_.shutdown(ec);
		}

		void close()
		{
			error_code ec;

			ssl_socket_.lowest_layer().close(ec);
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

		template <typename Response>
		auto query() -> awaitable<Response>
		{
			co_return co_await proto_.template query<Response>(this->shared_from_this());
		}

		auto sql()
		{
			return sql_;
		}

		template <typename Request, typename T>
		void make_request_buffer(std::shared_ptr<Request> request, flex_buffer<T>& buffer)
		{
			return proto_.template make_request_buffer<Request, T>(request, buffer);
		}

	private:
		ssl_socket ssl_socket_;

		std::size_t uuid_;

		protocol proto_;

		sql_adaptor<Server> sql_;

		timer_adaptor<boost::asio::steady_timer> timer_;
	};

	template <bool Server, typename Protocol>
	struct is_session_type<basic_session<Server, Protocol>> : std::true_type
	{};

	template<typename T>
	struct is_ssl_socket_type<boost::asio::ssl::stream<T>> : std::true_type {};
} // namespace aquarius