#define BOOST_TEST_NO_MAIN
#include <aquarius/ip/http/http_client.hpp>
#include <aquarius/ip/tcp/tcp_client.hpp>
#include <boost/test/unit_test.hpp>

using namespace std::chrono_literals;

BOOST_AUTO_TEST_SUITE(tcp_client_test_suite)

BOOST_AUTO_TEST_CASE(test_tcp_ctor)
{
	aquarius::io_context io;
	{
		aquarius::tcp_client client(io, 1000ms);

		BOOST_CHECK(client.get_executor() == io.get_executor());
		BOOST_TEST(client.get_timeout() == 1000ms);
	}

	{
		aquarius::tcp_client client(io.get_executor(), 1000ms);

		BOOST_CHECK(client.get_executor() == io.get_executor());
		BOOST_TEST(client.get_timeout() == 1000ms);
	}
}

BOOST_AUTO_TEST_CASE(test_http_ctor)
{
	aquarius::io_context io;
	{
		aquarius::http_client client(io, 1000ms);

		BOOST_CHECK(client.get_executor() == io.get_executor());
		BOOST_TEST(client.get_timeout() == 1000ms);
	}

	{
		aquarius::tcp_client client(io.get_executor(), 1000ms);

		BOOST_CHECK(client.get_executor() == io.get_executor());
		BOOST_TEST(client.get_timeout() == 1000ms);
	}
}

BOOST_AUTO_TEST_SUITE_END()