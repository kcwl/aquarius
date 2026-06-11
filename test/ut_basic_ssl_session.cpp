#define BOOST_TEST_NO_MAIN
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/error_code.hpp>
#include <boost/test/unit_test.hpp>

struct mock_failed_ssl_socket;
struct mock_ssl_socket;

namespace boost
{
	namespace asio
	{
		template <typename Resolver, typename Token>
		auto async_connect(mock_ssl_socket&, Resolver&&, Token&& token) -> asio::awaitable<void>
		{
			token.token_.ec_ = aquarius::error_code{};
			co_return;
		}

		template <typename Resolver, typename Token>
		auto async_connect(mock_failed_ssl_socket&, Resolver&&, Token&& token) -> asio::awaitable<void>
		{
			token.token_.ec_ = boost::asio::error::bad_descriptor;
			co_return;
		}

		template <typename BufferType, typename Token>
		auto async_read(mock_ssl_socket&, BufferType&&, Token&& token) -> asio::awaitable<void>
		{
			token.ec_ = aquarius::error_code{};
			co_return;
		}

		template <typename Token>
		auto async_read(mock_failed_ssl_socket&, aquarius::flex_buffer&, Token&& token) -> asio::awaitable<void>
		{
			token.ec_ = boost::asio::error::bad_descriptor;
			co_return;
		}

		template <typename Token>
		auto async_read_until(mock_ssl_socket&, aquarius::flex_buffer&, std::string_view, Token&& token)
			-> asio::awaitable<std::size_t>
		{
			token.ec_ = aquarius::error_code{};
			co_return 0;
		}

		template <typename Token>
		auto async_read_until(mock_failed_ssl_socket&, aquarius::flex_buffer&, std::string_view, Token&& token)
			-> asio::awaitable<std::size_t>
		{
			token.ec_ = boost::asio::error::bad_descriptor;
			co_return 0;
		}

		template <typename Token>
		auto async_write(mock_ssl_socket&, aquarius::flex_buffer&, Token&& token) -> asio::awaitable<void>
		{
			token.ec_ = aquarius::error_code{};
			co_return;
		}

		template <typename Token>
		auto async_write(mock_failed_ssl_socket&, aquarius::flex_buffer&, Token&& token) -> asio::awaitable<void>
		{
			token.ec_ = boost::asio::error::bad_descriptor;
			co_return;
		}
	} // namespace asio
} // namespace boost

#include <aquarius/basic_ssl_session.hpp>

using namespace std::chrono_literals;
using namespace aquarius;

struct mock_ssl_socket
{
	using lowest_layer_type = mock_ssl_socket;

	using executor_type = asio::any_io_executor;

	template <typename SSLContext>
	mock_ssl_socket(asio::io_context& io, SSLContext&)
		: io_(io)
	{}

	const auto get_executor() const
	{
		return io_.get_executor();
	}

	const mock_ssl_socket& remote_endpoint() const
	{
		return *this;
	}

	const mock_ssl_socket& lowest_layer() const
	{
		return *this;
	}

	mock_ssl_socket& lowest_layer()
	{
		return *this;
	}

	uint16_t port() const
	{
		return 3399u;
	}

	const mock_ssl_socket& address() const
	{
		return *this;
	}

	std::string to_string() const
	{
		return "127.0.0.1";
	}

	const mock_ssl_socket& to_v4() const
	{
		return *this;
	}

	uint32_t to_uint() const
	{
		return asio::ip::make_address("127.0.0.1").to_v4().to_uint();
	}

	void shutdown(error_code& ec)
	{
		ec = error_code{};
	}

	template <typename Option>
	void set_option(Option, error_code& ec)
	{
		ec = error_code{};
	}

	auto async_handshake() -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	template <typename Buffer, typename Token>
	auto async_write_some(Buffer&&, Token&&) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}
	template <typename Buffer, typename Token>
	auto async_read_some(Buffer&&, Token&&) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	template <typename Buffer>
	std::size_t read_some(Buffer&&, error_code& ec)
	{
		ec = error_code{};
		return 0;
	}

	template <typename Buffer>
	std::size_t write_some(Buffer&&, error_code& ec)
	{
		ec = error_code{};
		return 0;
	}

	asio::io_context& io_;
};

struct mock_failed_ssl_socket
{
	using lowest_layer_type = mock_failed_ssl_socket;
	using executor_type = asio::any_io_executor;

	template <typename SSLContext>
	mock_failed_ssl_socket(asio::io_context& io, SSLContext&)
		: io_(io)
	{}

	void shutdown(error_code& ec)
	{
		ec = boost::asio::error::bad_descriptor;
	}

	template <typename Option>
	void set_option(Option, error_code& ec)
	{
		ec = boost::asio::error::bad_descriptor;
	}

	const mock_failed_ssl_socket& lowest_layer() const
	{
		return *this;
	}

	mock_failed_ssl_socket& lowest_layer()
	{
		return *this;
	}

	auto async_handshake() -> asio::awaitable<error_code>
	{
		co_return boost::asio::error::bad_descriptor;
	}

	template <typename Buffer, typename Token>
	auto async_write_some(Buffer&&, Token&&) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}
	template <typename Buffer, typename Token>
	auto async_read_some(Buffer&&, Token&&) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	const auto get_executor() const
	{
		return io_.get_executor();
	}

	template <typename Buffer>
	std::size_t read_some(Buffer&&, error_code& ec)
	{
		ec = error_code{};
		return 0;
	}

	template <typename Buffer>
	std::size_t write_some(Buffer&&, error_code& ec)
	{
		ec = error_code{};
		return 0;
	}

	asio::io_context& io_;
};

BOOST_AUTO_TEST_SUITE(ut_basic_ssl_session)

struct mock_ssl_protocol
{
	using socket_type = mock_ssl_socket;

	using resolver_type = asio::ip::tcp::resolver;

	using no_delay = asio::ip::tcp::no_delay;

	using keep_alive = asio::socket_base::keep_alive;
};

struct mock_ssl_context
{};

BOOST_AUTO_TEST_CASE(ctor)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_ssl_protocol::socket_type(io, ctx);
	auto executor = socket.get_executor();
	basic_ssl_session<mock_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	BOOST_CHECK(executor == session.get_executor());
	BOOST_TEST(session.uuid() != 0);
	BOOST_TEST(session.timeout() == 10ms);
}

BOOST_AUTO_TEST_CASE(remote_address)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_ssl_protocol::socket_type(io, ctx);
	auto executor = socket.get_executor();
	basic_ssl_session<mock_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	BOOST_TEST(session.remote_address() == "127.0.0.1");
	BOOST_TEST(session.remote_address_u() == 2130706433u);
	BOOST_TEST(session.remote_port() == 3399);
}

struct mock_failed_ssl_protocol
{
	using socket_type = mock_failed_ssl_socket;

	using resolver_type = asio::ip::tcp::resolver;

	using no_delay = asio::ip::tcp::no_delay;

	using keep_alive = asio::socket_base::keep_alive;
};

BOOST_AUTO_TEST_CASE(async_connect_failed)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_failed_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_failed_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	auto future = asio::co_spawn(
		io,
		[&]() mutable -> asio::awaitable<void>
		{
			auto ec = co_await session.async_connect("127.0.0.1", "3399");

			BOOST_TEST(ec == boost::asio::error::bad_descriptor);
		},
		asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_CASE(async_connect_success)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	auto future = asio::co_spawn(
		io,
		[&]() mutable -> asio::awaitable<void>
		{
			auto ec = co_await session.async_connect("127.0.0.1", "3399");

			BOOST_TEST(!ec);
		},
		asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_CASE(async_handshake_failed)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_failed_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_failed_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	auto future = asio::co_spawn(
		io,
		[&]() mutable -> asio::awaitable<void>
		{
			auto ec = co_await session.async_handshake();

			BOOST_TEST(ec == boost::asio::error::bad_descriptor);
		},
		asio::use_future);

	io.run();

	BOOST_REQUIRE_THROW(future.get(), std::exception);
}

BOOST_AUTO_TEST_CASE(async_handshake_success)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	auto future = asio::co_spawn(
		io,
		[&]() mutable -> asio::awaitable<void>
		{
			auto ec = co_await session.async_handshake();

			BOOST_TEST(!ec);
		},
		asio::use_future);

	io.run();

	BOOST_REQUIRE_THROW(future.get(), std::exception);
}

BOOST_AUTO_TEST_CASE(async_read_failed)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_failed_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_failed_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	auto future = asio::co_spawn(
		io,
		[&]() mutable -> asio::awaitable<void>
		{
			flex_buffer buffer{};
			auto ec = co_await session.async_read(buffer, 0);

			BOOST_TEST(ec == boost::asio::error::bad_descriptor);
		},
		asio::use_future);

	io.run();

	BOOST_REQUIRE_THROW(future.get(), std::exception);
}

BOOST_AUTO_TEST_CASE(async_read_success)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	auto future = asio::co_spawn(
		io,
		[&]() mutable -> asio::awaitable<void>
		{
			flex_buffer buffer{};
			auto ec = co_await session.async_read(buffer, 0);

			BOOST_TEST(!ec);
		},
		asio::use_future);

	io.run();

	BOOST_REQUIRE_THROW(future.get(), std::exception);
}
BOOST_AUTO_TEST_CASE(async_read_untl_failed)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_failed_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_failed_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	auto future = asio::co_spawn(
		io,
		[&]() mutable -> asio::awaitable<void>
		{
			flex_buffer buffer{};
			std::size_t endpos{};
			auto ec = co_await session.async_read_until(buffer, "", endpos);

			BOOST_TEST(ec == boost::asio::error::bad_descriptor);
		},
		asio::use_future);

	io.run();

	BOOST_REQUIRE_THROW(future.get(), std::exception);
}

BOOST_AUTO_TEST_CASE(async_read_untl_success)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	auto future = asio::co_spawn(
		io,
		[&]() mutable -> asio::awaitable<void>
		{
			flex_buffer buffer{};
			std::size_t endpos{};
			auto ec = co_await session.async_read_until(buffer, "", endpos);

			BOOST_TEST(!ec);
		},
		asio::use_future);

	io.run();

	BOOST_REQUIRE_THROW(future.get(), std::exception);
}
BOOST_AUTO_TEST_CASE(async_send_failed)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_failed_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_failed_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	auto future = asio::co_spawn(
		io,
		[&]() mutable -> asio::awaitable<void>
		{
			flex_buffer buffer{};
			auto ec = co_await session.async_send(buffer);

			BOOST_TEST(ec == boost::asio::error::bad_descriptor);
		},
		asio::use_future);

	io.run();

	BOOST_REQUIRE_THROW(future.get(), std::exception);
}

BOOST_AUTO_TEST_CASE(async_send_success)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	auto future = asio::co_spawn(
		io,
		[&]() mutable -> asio::awaitable<void>
		{
			flex_buffer buffer{};
			auto ec = co_await session.async_send(buffer);

			BOOST_TEST(!ec);
		},
		asio::use_future);

	io.run();

	BOOST_REQUIRE_THROW(future.get(), std::exception);
}

BOOST_AUTO_TEST_CASE(shutdown_failed)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_failed_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_failed_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	BOOST_TEST(!session.shutdown());
}

BOOST_AUTO_TEST_CASE(shutdown_success)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	BOOST_TEST(!session.shutdown());
}

BOOST_AUTO_TEST_CASE(keep_alive_failed)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_failed_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_failed_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	BOOST_TEST(session.keep_alive(true));
}

BOOST_AUTO_TEST_CASE(keep_alive_success)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	BOOST_TEST(!session.keep_alive(true));
}

BOOST_AUTO_TEST_CASE(set_nodelay_failed)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_failed_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_failed_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	BOOST_TEST(session.set_nodelay(true));
}

BOOST_AUTO_TEST_CASE(set_nodelay_success)
{
	asio::io_context io{};
	mock_ssl_context ctx{};
	auto socket = mock_ssl_protocol::socket_type(io, ctx);

	basic_ssl_session<mock_ssl_protocol, asio::ssl::context::tlsv13> session(std::move(socket), 10ms);

	BOOST_TEST(!session.set_nodelay(true));
}

BOOST_AUTO_TEST_SUITE_END()
