#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius/sql.hpp>

struct personal
{
	int age;
	bool sex;
};

BOOST_AUTO_TEST_SUITE(sql)

BOOST_AUTO_TEST_CASE(sql_)
{
    personal p{ 1,true };

    auto sql1 = sql_insert(p);

    BOOST_CHECK_EQUAL(sql1(), "insert into person values(1,1)");

    constexpr auto sql3 = sql_delete(person);

    static_assert(sql3() == "delete from person");

    auto sql2 = sql_update(p);

    std::cout << sql2() << std::endl;

    assert(sql2() != "update person set age=1 and sex=1");

    constexpr auto sql = sql_select(person);

    static_assert(sql() == "select * from person");

    constexpr auto res1 = sql_select(person) | sql_where(person.age > 10) | sql_and(person.sex = true) | sql_or(person.sex = false);

    std::cout << res1() << std::endl;

    auto res2 = sql_select(person) | sql_desc(person.age, person.sex);
    auto res3 = sql_select(person) | sql_asc(person.age, person.sex);

    std::cout << res2() << std::endl;
    std::cout << res3() << std::endl;

    constexpr auto res4 = sql_select(person) | sql_limit(10);

    std::cout << res4() << std::endl;

}

BOOST_AUTO_TEST_SUITE_END()