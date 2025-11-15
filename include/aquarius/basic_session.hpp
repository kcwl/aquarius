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
#include <aquarius/adaptor/sql_adaptor.hpp>
#include <aquarius/adaptor/timer_adaptor.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <aquarius/ip/tcp/tcp_error.hpp>
#include <aquarius/ip/concept.hpp>
#include <aquarius/ip/socket_adaptor.hpp>
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{
	template <bool Server, template<bool> typename Protocol>
	class basic_session : public std::enable_shared_from_this<basic_session<Server, Protocol>>
	{
	public:
		using protocol = Protocol<Server>;

		constexpr static auto is_server = Server;

		using socket = protocol::socket;

		using endpoint = protocol::endpoint;

		using acceptor = protocol::acceptor;

		using resolver = protocol::resolver;

	public:
		explicit basic_session(socket sock)
			: socket_(std::move(sock))
			, socket_adaptor_(socket_)
			, uuid_(detail::uuid_generator()())
			, proto_()
			, sql_(socket_.get_executor())
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

		auto async_read(flex_buffer& buffer) -> awaitable<error_code>
		{
			error_code ec;

			co_await boost::asio::async_read(socket_adaptor_.get_implement(), buffer,
											 redirect_error(use_awaitable, ec));

			co_return ec;
		}

		auto async_read_util(flex_buffer& buffer, std::string_view delm) -> awaitable<error_code>
		{
			error_code ec;

			co_await boost::asio::async_read_until(socket_adaptor_.get_implement(), buffer, delm,
												   redirect_error(use_awaitable, ec));

			co_return ec;
		}

		auto async_send(const flex_buffer& buffer) -> awaitable<error_code>
		{
			error_code ec{};

			co_await socket_adaptor_.get_implement().async_write_some(buffer.data(), redirect_error(use_awaitable, ec));

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

		auto sql()
		{
			return sql_;
		}

	private:
		socket socket_;

		socket_adaptor<Server, protocol> socket_adaptor_;

		std::size_t uuid_;

		protocol proto_;

		sql_adaptor<Server> sql_;

		timer_adaptor<boost::asio::steady_timer> timer_;
	};

	template <bool Server, template<bool> typename Protocol>
	struct is_session_type<basic_session<Server, Protocol>> : std::true_type
	{};
} // namespace aquarius