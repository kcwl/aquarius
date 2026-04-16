#define BOOST_TEST_NO_MAIN
#include <aquarius/ip/http.hpp>
#include <aquarius/ip/tcp.hpp>
#include <boost/test/unit_test.hpp>

using namespace std::chrono_literals;
using namespace aquarius;

struct mock_normal_session
{
	using socket = asio::ip::tcp::socket;
	using resolver = asio::ip::tcp::resolver;

	template <typename... Args>
	mock_normal_session(Args&&...)
	{}

	auto async_connect(const std::string& host, const std::string& port) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	auto async_send(flex_buffer) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	auto query() -> asio::awaitable<std::expected<flex_buffer, error_code>>
	{
		flex_buffer buffer{};
		co_return buffer;
	}

	int remote_port() { return 0; }

	std::string remote_address() { return ""; }

	void close() {}
};

struct mock_error_session
{
	using socket = asio::ip::tcp::socket;
	using resolver = asio::ip::tcp::resolver;

	template <typename... Args>
	mock_error_session(Args&&...)
	{}

	auto async_connect(const std::string& host, const std::string& port) -> asio::awaitable<error_code>
	{
		co_return asio::error::eof;
	}

	auto async_send(flex_buffer) -> asio::awaitable<error_code>
	{
		co_return asio::error::eof;
	}

	auto query() -> asio::awaitable<std::expected<flex_buffer, error_code>>
	{
		flex_buffer buffer{};
		co_return buffer;
	}

	void close() {}
};

struct mock_request
{
	bool commit(flex_buffer&)
	{
		return true;
	}
	bool consume(flex_buffer&)
	{
		return true;
	}
};
struct mock_response
{
	bool commit(flex_buffer&)
	{
		return true;
	}
	bool consume(flex_buffer&)
	{
		return true;
	}
};

using client = basic_client<mock_normal_session>;
using error_client = basic_client<mock_error_session>;

BOOST_AUTO_TEST_SUITE(ut_client)

BOOST_AUTO_TEST_CASE(ctor)
{
	asio::io_context io{};
	client cli{ io, 2s };

	BOOST_REQUIRE(cli.get_executor() == io.get_executor());
	BOOST_TEST(cli.get_timeout() == 2ms);
}

BOOST_AUTO_TEST_CASE(connect_success)
{
	asio::io_context io{};

	client cli{ io, 2s };

	auto future = asio::co_spawn(
		io,
		[&]() -> asio::awaitable<void>
		{
			auto ec = co_await cli.async_connect("", 0);
			BOOST_TEST(!ec);

			flex_buffer buffer{};

			ec = co_await cli.async_send(std::move(buffer));

			BOOST_TEST(!ec);

			auto req_ptr = std::make_shared<mock_request>();

			BOOST_CHECK_NO_THROW(co_await cli.async_send<mock_response>(req_ptr));
		},
		asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_CASE(connect_failed)
{
	asio::io_context io{};

	error_client cli{ io, 2s };

	auto future = asio::co_spawn(
		io,
		[&]() -> asio::awaitable<void>
		{
			auto ec = co_await cli.async_connect("", 0);
			BOOST_TEST(ec);

			flex_buffer buffer{};

			ec = co_await cli.async_send(std::move(buffer));

			BOOST_TEST(ec);

			auto req_ptr = std::make_shared<mock_request>();

			BOOST_CHECK_NO_THROW(co_await cli.async_send<mock_response>(req_ptr));
		},
		asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_CASE(remote_info)
{
	asio::io_context io{};

	client cli(io, 2s);

	BOOST_TEST(cli.remote_address() == "127.0.0.1");

	BOOST_TEST(cli.remote_port() == 7890);

	auto future =
		asio::co_spawn(io, [&]() -> asio::awaitable<void> { BOOST_TEST(!co_await cli.reconnect()); }, asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_CASE(callback)
{
	asio::io_context io{};

	client cli(io, 2s);

	int value = 0;

	cli.accept_invoke();

	BOOST_TEST(value == 0);

	cli.set_accept_func([&](auto) { value++; });

	BOOST_TEST(value == 1);

	cli.close_invoke();

	BOOST_TEST(value == 1);

	cli.set_close_func([&](auto) { value--; });

	BOOST_TEST(value == 0);
}

BOOST_AUTO_TEST_SUITE_END()