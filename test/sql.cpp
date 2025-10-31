#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
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

    BOOST_TEST(sql1() == "insert into personal values(1,1)");

    constexpr auto sql3 = sql_delete(personal);

    static_assert(sql3() == "delete from personal");

    auto sql2 = sql_update(p);

    BOOST_TEST(sql2() == "update personal set age = 1 and sex = 1");

    constexpr auto sql = sql_select(personal);

    static_assert(sql() == "select * from personal");

    constexpr auto res1 = sql_select(personal) | sql_where(personal.age > 10) | sql_and(personal.sex = true);

    static_assert(res1() == "select * from personal where age > 10 and sex=true");

    auto res2 = sql_select(personal) | sql_desc(personal.age, personal.sex);

    static_assert(res2() == "select * from personal order by personal.age, personal.sex desc");

    auto res3 = sql_select(personal) | sql_asc(personal.age, personal.sex);

    static_assert(res3() == "select * from personal order by personal.age, personal.sex asc");

    constexpr auto res4 = sql_select(personal) | sql_limit(10);

    static_assert(res4() == "select * from personal limit 10");
}

BOOST_AUTO_TEST_SUITE_END()