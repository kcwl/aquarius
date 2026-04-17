#define BOOST_TEST_NO_MAIN
#include <aquarius.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;


BOOST_AUTO_TEST_SUITE(ut_tcp)


struct mock_read_error_session
{
	mock_read_error_session(asio::io_context& context)
		: io(context)
	{

	}

	auto get_executor()
	{
		return io.get_executor();
	}

	auto async_read(flex_buffer& buffer, std::size_t bytes) -> asio::awaitable<error_code>
	{
		co_return asio::error::bad_descriptor;
	}

	auto async_send(flex_buffer) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	asio::io_context& io;
};

struct mock_second_read_error_session
{
	mock_second_read_error_session(asio::io_context& context)
		: io(context)
	{

	}

	auto get_executor()
	{
		return io.get_executor();
	}

	auto async_read(flex_buffer& buffer, std::size_t bytes) -> asio::awaitable<error_code>
	{
		error_code ec{};

		if (!has_run)
		{
			has_run = true;
		}
		else
		{
			ec = asio::error::bad_descriptor;
		}

		co_return ec;
	}

	auto async_send(flex_buffer) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	bool has_run;
	asio::io_context& io;
};

template <typename T>
struct tcp_accept_session_test
{
	void operator()(error_code ec)
	{
		asio::io_context io{};

		auto session = std::make_shared<T>(io);

		auto future = asio::co_spawn(
			io, [session] () -> asio::awaitable<error_code> { co_return co_await tcp::accept(session); },
			asio::use_future);

		io.run();

		BOOST_TEST(future.get() == ec);
	}
};
template <typename T>
struct tcp_query_session_test
{
	void operator()(error_code ec)
	{
		asio::io_context io{};

		auto session = std::make_shared<T>(io);

		auto future = asio::co_spawn(
			io, [session] () -> asio::awaitable<std::expected<flex_buffer, error_code>> { co_return co_await tcp::query(session); },
			asio::use_future);

		io.run();

		auto result = future.get();

		BOOST_TEST(result.error() == ec);
	}
};

BOOST_AUTO_TEST_CASE(accepts_read_error)
{
	tcp_accept_session_test<mock_read_error_session>()(asio::error::bad_descriptor);
}

BOOST_AUTO_TEST_CASE(accepts_second_read_error)
{
	tcp_accept_session_test<mock_second_read_error_session>()(asio::error::bad_descriptor);
}
BOOST_AUTO_TEST_CASE(query_read_error)
{
	tcp_query_session_test<mock_read_error_session>()(asio::error::bad_descriptor);
}

BOOST_AUTO_TEST_CASE(query_second_read_error)
{
	tcp_query_session_test<mock_second_read_error_session>()(asio::error::bad_descriptor);
}

BOOST_AUTO_TEST_SUITE_END()