#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius.hpp>

BOOST_AUTO_TEST_SUITE(error_code)

BOOST_AUTO_TEST_CASE(msg)
{
	aquarius::error_code ec{};

	BOOST_CHECK(ec.message() == "success");

	BOOST_CHECK(ec.category() == "aquarius system");

	ec = aquarius::system::system_errc::invalid_proto;

	BOOST_CHECK(ec.message() == "maybe the protocal is not registed");
	
	BOOST_CHECK(ec.value() == 1);

	ec = 6;

	BOOST_CHECK(ec.message() == "unknown error");

	ec = 2;

	BOOST_CHECK(ec.message() == "the buffer is not completed");

	ec = 3;

	BOOST_CHECK(ec.message() == "the stream is not completed");

	ec = 4;

	BOOST_CHECK(ec.message() == "maybe context is not complete");

	ec = 5;

	BOOST_CHECK(ec.message() == "maybe the message is invalid");
}

BOOST_AUTO_TEST_CASE(category)
{
	aquarius::system::system_error_category sec{};

	auto condition = sec.default_error_condition(0);

	BOOST_CHECK(condition.value() == 0);

	BOOST_CHECK(std::string(condition.category().name()) == "aquarius system");
}

BOOST_AUTO_TEST_SUITE_END()