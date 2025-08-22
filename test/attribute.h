#pragma once
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/attribute.hpp>

BOOST_AUTO_TEST_SUITE(attr)

BOOST_AUTO_TEST_CASE(equal)
{
	auto str = aquarius::attributes<"age">() == 1;

	BOOST_CHECK_EQUAL(str.sql(), " age=1");
}

BOOST_AUTO_TEST_CASE(not_equal)
{
	auto str = AQUARIUS_EXPR(age) != 1;

	BOOST_CHECK_EQUAL(str.sql(), " age!=1");
}

BOOST_AUTO_TEST_CASE(less)
{
	auto str = AQUARIUS_EXPR(age) < 1;

	BOOST_CHECK_EQUAL(str.sql(), " age<1");
}

BOOST_AUTO_TEST_CASE(less_equal)
{
	auto str = AQUARIUS_EXPR(age) <= 1;

	BOOST_CHECK_EQUAL(str.sql(), " age<=1");
}

BOOST_AUTO_TEST_CASE(greate)
{
	auto str = AQUARIUS_EXPR(age) > 1;

	BOOST_CHECK_EQUAL(str.sql(), " age>1");
}

BOOST_AUTO_TEST_CASE(greate_equal)
{
	auto str = AQUARIUS_EXPR(age) >= 1;

	BOOST_CHECK_EQUAL(str.sql(), " age>=1");
}

BOOST_AUTO_TEST_CASE(_or)
{
	auto other = AQUARIUS_EXPR(age) >= 1;

	auto left = AQUARIUS_EXPR(age) < 3;

	auto str = left | other;

	BOOST_CHECK_EQUAL(str.sql(), " age<3 or age>=1");
}

BOOST_AUTO_TEST_CASE(_and)
{
	auto other = AQUARIUS_EXPR(name) == "a";

	auto left = AQUARIUS_EXPR(age) < 3;

	auto str =  left & other;

	BOOST_CHECK_EQUAL(str.sql(), " age<3 and name=\"a\"");
}

BOOST_AUTO_TEST_SUITE_END()