#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius/elastic.hpp>

BOOST_AUTO_TEST_SUITE(tuple_size)

struct person
{
	int a;
	char b;
};

BOOST_AUTO_TEST_CASE(tuple_size)
{
	static_assert(aquarius::tuple_size_v<person> == 2, "size error!");

	static_assert(aquarius::tuple_size_v<std::array<int, 4>> == 4, "size error!");

	static_assert(aquarius::tuple_size_v<std::tuple<int, char>> == 2, "size error!");
}

BOOST_AUTO_TEST_CASE(reflect)
{
	constexpr person t3{ 1, 'a' };

	static_assert(aquarius::get<0>(t3) == 1, "get value error!");
	static_assert(aquarius::get<1>(t3) == 'a', "get value error!");
	static_assert(aquarius::name<person>() == "person"sv, "get name error!");
}

BOOST_AUTO_TEST_SUITE_END()
