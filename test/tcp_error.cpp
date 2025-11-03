#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/ip/tcp/tcp_error.hpp>

BOOST_AUTO_TEST_SUITE(tcps)

BOOST_AUTO_TEST_CASE(errors)
{
	{
		aquarius::error_code ec = aquarius::tcp_error::success;

		BOOST_TEST(ec.message() == "success");
	}

	{
		aquarius::error_code ec = aquarius::tcp_error::no_space;

		BOOST_TEST(ec.message() == "no space");
	}

	{
		aquarius::error_code ec = aquarius::tcp_error::no_space;

		BOOST_TEST(ec.category().name() == "serialize error category");
	}
}

BOOST_AUTO_TEST_SUITE_END()