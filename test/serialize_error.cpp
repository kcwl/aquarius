#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/serialize/error.hpp>

BOOST_AUTO_TEST_SUITE(serialize)

BOOST_AUTO_TEST_CASE(error)
{
	aquarius::error_code ec = aquarius::serialize_error::success;

	BOOST_TEST(ec.message() == "success");

	ec = aquarius::serialize_error::not_enough_space;

	BOOST_TEST(ec.message() == "not enough space");

	ec = aquarius::serialize_error::overflow;

	BOOST_TEST(ec.message() == "overflow");
}

BOOST_AUTO_TEST_SUITE_END()