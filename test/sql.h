#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius/detail/sql_stream.hpp>
#include <aquarius/detail/attribute.hpp>

struct personal
{
	int age;
	bool sex;
	std::string name;
};

BOOST_AUTO_TEST_SUITE(sql)

BOOST_AUTO_TEST_CASE(insert)
{
	personal p{1, true, "per"};

	auto sql_str = aquarius::sql_stream<>().insert(p).sql();

	BOOST_CHECK_EQUAL(sql_str, "insert into personal values(1,1,\"per\")");
}

BOOST_AUTO_TEST_CASE(remove)
{
	personal p{ 1, true, "per" };

	auto sql_str = aquarius::sql_stream<>().remove(p).sql();

	BOOST_CHECK_EQUAL(sql_str, "delete from personal");
}

BOOST_AUTO_TEST_CASE(update)
{
	personal p{ 1, true, "per" };

	auto sql_str = aquarius::sql_stream<>().update(p).sql();

	BOOST_CHECK_EQUAL(sql_str, "update personal set age=1 and sex=1 and name=\"per\"");
}

BOOST_AUTO_TEST_CASE(select)
{
	auto sql_str = aquarius::sql_stream<>().select<personal>().sql();

	BOOST_CHECK_EQUAL(sql_str, "select * from personal");
}

BOOST_AUTO_TEST_CASE(where)
{
	auto sql_str = aquarius::sql_stream<>().select<personal>().where(AQUARIUS_EXPR(age) > 2).sql();

	BOOST_CHECK_EQUAL(sql_str, "select * from personal where age>2");
}

BOOST_AUTO_TEST_CASE(_and)
{
	auto sql_str = aquarius::sql_stream<>().select<personal>().where(AQUARIUS_EXPR(age) > 2)._and(AQUARIUS_EXPR(sex) == 0).sql();

	BOOST_CHECK_EQUAL(sql_str, "select * from personal where age>2 and sex=0");
}

BOOST_AUTO_TEST_CASE(_or)
{
	auto sql_str = aquarius::sql_stream<>().select<personal>().where(AQUARIUS_EXPR(age) > 2)._or(AQUARIUS_EXPR(sex) == 0).sql();

	BOOST_CHECK_EQUAL(sql_str, "select * from personal where age>2 or sex=0");
}

BOOST_AUTO_TEST_CASE(order_by)
{
	{
		constexpr static auto column = "age"sv;
		auto sql_str = aquarius::sql_stream<>().select<personal>().order_by<true, column>().sql();

		BOOST_CHECK_EQUAL(sql_str, "select * from personal order by age asc");
	}

	{
		constexpr static auto column = "age"sv;
		auto sql_str = aquarius::sql_stream<>().select<personal>().order_by<false, column>().sql();

		BOOST_CHECK_EQUAL(sql_str, "select * from personal order by age desc");
	}
}

BOOST_AUTO_TEST_SUITE_END()