#pragma once
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <span>
#include <aquarius/detail/ssl_context.hpp>
#include <boost/asio/read.hpp>
#include <aquarius/coroutine.hpp>
#include <boost/asio/connect.hpp>
#include <aquarius/detail/redirect_error.hpp>
#include <aquarius/adaptor/timer_adaptor.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <aquarius/ip/tcp/tcp_error.hpp>
#include <aquarius/ip/concept.hpp>
#include <aquarius/ip/socket_adaptor.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/sql/sql_connector.hpp>

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

		using executor_type = typename socket::executor_type;

		using sql_conn_t = sql_connector<executor_type>;

	public:
		explicit basic_session(socket sock)
			: socket_(std::move(sock))
			, socket_adaptor_(socket_)
			, uuid_(detail::uuid_generator()())
			, proto_()
			, timer_(socket_.get_executor())
		{}

		basic_session(io_context& io)
			: basic_session(std::move(socket(io)))
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

		auto async_connect(const std::string& host, const std::string& port) -> awaitable<error_code>
		{
			co_return co_await socket_adaptor_.async_connect(host, port);
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

		auto async_read(flex_buffer& buffer, std::size_t length) -> awaitable<error_code>
		{
			error_code ec;

			if (length > buffer.remain())
			{
				co_return ec = tcp_error::no_space;
			}

			co_return co_await async_read(buffer.wdata(), length);
		}

		auto async_read(uint8_t* begin, std::size_t length) -> awaitable<error_code>
		{
			error_code ec;

			co_await boost::asio::async_read(socket_adaptor_.get_implement(), boost::asio::buffer(begin, length),
											 redirect_error(use_awaitable, ec));

			co_return ec;
		}

		auto async_read_util(flex_buffer& buffer, std::string_view delm) -> awaitable<error_code>
		{
			error_code ec;

			boost::asio::streambuf buf{};

			co_await boost::asio::async_read_until(socket_adaptor_.get_implement(), buf, delm,
												   redirect_error(use_awaitable, ec));

			if (!ec)
			{
				buffer.put((uint8_t*)buf.data().data(), buf.size());
			}

			co_return ec;
		}

		auto async_send(flex_buffer buffer) -> awaitable<error_code>
		{
			error_code ec{};

			co_await socket_adaptor_.get_implement().async_write_some(
				boost::asio::buffer(buffer.rdata(), buffer.active()), redirect_error(use_awaitable, ec));

			co_return ec;
		}

		void shutdown()
		{
			error_code ec;

			socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);
		}

		void close()
		{
			error_code ec;

			socket_.close(ec);
		}

		auto accept() -> awaitable<void>
		{
			co_return co_await socket_adaptor_.accept([this] -> awaitable<void>
													  { co_return co_await proto_.accept(this->shared_from_this()); });
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

		template <typename Response>
		auto query() -> awaitable<Response>
		{
			co_return co_await proto_.template query<Response>(this->shared_from_this());
		}

		void attach_sql(std::shared_ptr<sql_conn_t> sql_ptr)
		{
			sql_conn_ptr_ = sql_ptr;
		}

	private:
		socket socket_;

		socket_adaptor<Server, Protocol> socket_adaptor_;

		std::size_t uuid_;

		protocol proto_;

		timer_adaptor<boost::asio::steady_timer> timer_;

		std::weak_ptr<sql_conn_t> sql_conn_ptr_;
	};

	template <bool Server, typename Protocol>
	struct is_session_type<basic_session<Server, Protocol>> : std::true_type
	{};
} // namespace aquarius