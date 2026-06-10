#define BOOST_TEST_NO_MAIN
#include <aquarius/basic_server.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_basic_server)
struct mock_acceptor
{
	template <typename Endpoint>
	mock_acceptor(asio::io_context& ios, Endpoint&&)
		: io_(ios)
	{}

	auto get_executor()
	{
		return io_.get_executor();
	}

	void close()
	{}

	template <typename Token>
	auto async_accept(asio::io_context& io, Token&& k) -> asio::awaitable<asio::ip::tcp::socket>
	{
		k.ec_ = error_code{};
		co_return std::move(asio::ip::tcp::socket(io));
	}

	asio::io_context& io_;
};

struct mock_session
{
	using acceptor_type = mock_acceptor;

	template <typename Endpoint>
	mock_session(asio::ip::tcp::socket, Endpoint&&)
	{}

	std::size_t uuid() const
	{
		return 0;
	}

	void close()
	{}

	void keep_alive(bool)
	{}

	void nodelay(bool)
	{}

	auto async_send_with_header(flex_buffer&) -> asio::awaitable<void>
	{
		co_return;
	}

	auto accept() -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}
};

// BOOST_AUTO_TEST_CASE(ctor)
//{
//	basic_server<mock_session> srv(9800, 10, "mock server", 40ms);
// }

BOOST_AUTO_TEST_CASE(server_start_and_close)
{
	basic_server<mock_session> srv(9800, 10, "mock server", 40ms);

	std::thread t([&]() mutable { BOOST_REQUIRE_NO_THROW(srv.run()); });

	BOOST_REQUIRE_NO_THROW(srv.stop());

	t.join();
}

BOOST_AUTO_TEST_CASE(server_invoke)
{
	basic_server<mock_session> srv(9800, 10, "mock server", 40ms);

	srv.set_accept_func(
		[](auto session) -> asio::awaitable<void>
		{
			BOOST_TEST(session != nullptr);
			co_return;
		});
	srv.set_close_func(
		[](auto session) -> asio::awaitable<void>
		{
			BOOST_TEST(session != nullptr);
			co_return;
		});

	asio::io_context io{};

	auto future = asio::co_spawn(
		io,
		[&]() mutable -> asio::awaitable<void>
		{
			co_await srv.accept_func(nullptr);

			co_await srv.accept_func(std::make_shared<mock_session>(std::move(asio::ip::tcp::socket(io)), 10ms));

			co_await srv.close_func(nullptr);

			co_await srv.close_func(std::make_shared<mock_session>(std::move(asio::ip::tcp::socket(io)), 10ms));
		},
		asio::use_future);

	io.run();

	future.get();
}

struct mock_failed_acceptor
{
	template <typename Token>
	auto async_accept(asio::io_context&, asio::redirect_error_t<Token>& k) -> asio::awaitable<asio::ip::tcp::socket>
	{
		k.ec_ = boost::asio::error::bad_descriptor;

		co_return nullptr;
	}
};

struct mock_failed_session
{
	template <typename Endpoint>
	mock_failed_session(asio::ip::tcp::socket, Endpoint&&)
	{}

	using acceptor_type = mock_acceptor;

	std::size_t uuid() const
	{
		return 0;
	}

	void close()
	{}

	void keep_alive(bool)
	{}

	void nodelay(bool)
	{}

	auto async_send_with_header(flex_buffer&) -> asio::awaitable<void>
	{
		co_return;
	}

	auto accept() -> asio::awaitable<error_code>
	{
		co_return boost::asio::error::bad_descriptor;
	}
};

BOOST_AUTO_TEST_CASE(server_accept_failed)
{
	basic_server<mock_failed_session> srv(9800, 10, "mock server", 40ms);

	std::thread t([&] ()
				  {
					  BOOST_REQUIRE_NO_THROW(srv.run());
				  });

	srv.stop();

	t.join();
}

struct mock_success_acceptor
{
	template <typename Token>
	auto async_accept(asio::io_context&, asio::redirect_error_t<Token>& k) -> asio::awaitable<asio::ip::tcp::socket>
	{
		k.ec_ = error_code{};

		co_return nullptr;
	}
};

struct mock_accept_failed_session
{
	using acceptor_type = mock_acceptor;

	template <typename Endpoint>
	mock_accept_failed_session(asio::ip::tcp::socket, Endpoint&&)
	{}

	auto accept() -> asio::awaitable<error_code>
	{
		co_return boost::asio::error::bad_descriptor;
	}

	void keep_alive(bool)
	{}

	void nodelay(bool)
	{}

	void close()
	{}

	std::size_t uuid() const
	{
		return 0;
	}

	auto async_send_with_header(flex_buffer&) -> asio::awaitable<void>
	{
		co_return;
	}
};

BOOST_AUTO_TEST_CASE(server_accept_success_and_query_failed)
{
	basic_server<mock_accept_failed_session> srv(9800, 10, "mock server", 40ms);

	std::thread t([&] ()
				  {
					  BOOST_REQUIRE_NO_THROW(srv.run());
				  });

	srv.stop();

	t.join();
}

// BOOST_AUTO_TEST_CASE(init_signal_failed)
//{
//	basic_server<mock_success_session> srv(9800, 10, "mock server", 40ms);
//
//	BOOST_REQUIRE_NO_THROW(srv.run());
// }
//
// BOOST_AUTO_TEST_CASE(init_global_timer_failed)
//{
//	basic_server<mock_success_session> srv(9800, 10, "mock server", 40ms);
//
//	BOOST_REQUIRE_NO_THROW(srv.run());
// }

BOOST_AUTO_TEST_SUITE_END()
