#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/sql.hpp>
#include <aquarius/tag_invoke.hpp>
#include <aquarius/sql/database_param.hpp>
#include <aquarius/asio.hpp>
#include <aquarius/sql/sql_connector.hpp>

struct personal
{
	int age;
	bool sex;
};

//namespace aquarius
//{
//	void tag_invoke(aquarius::value_to_tag<aquarius::db_tag>, aquarius::database_param& param)
//	{
//		param.host = "localhost";
//		param.user = "kcwl";
//		param.password = "NN0705lwl1217&";
//		param.db = "unittest";
//	}
//}


BOOST_AUTO_TEST_SUITE(sql)

BOOST_AUTO_TEST_CASE(sql_)
{
	personal p{ 1, true };

	auto sql1 = sql_insert(p);

	BOOST_TEST(sql1() == "insert into personal values(1,1)");

	constexpr auto sql3 = sql_delete(personal);

	static_assert(sql3() == "delete from personal");

	auto sql2 = sql_update(p);

	BOOST_TEST(sql2() == "update personal set age=1 and sex=1");

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

//BOOST_AUTO_TEST_CASE(connecting)
//{
//	personal p{ 1, true };
//
//	aquarius::io_context io{};
//
//	aquarius::database_param db_param{};
//
//	aquarius::value_to<aquarius::db_tag>(db_param);
//
//	aquarius::sql_connector<aquarius::io_context::executor_type> sql_conn(io, db_param);
//
//	auto future = aquarius::co_spawn(io, [&] ->aquarius::awaitable<void>
//									 {
//										 auto ec = co_await sql_conn.async_connect();
//
//										 BOOST_TEST(!ec);
//
//										 auto res = co_await sql_conn.async_insert(sql_insert(p)());
//
//										 BOOST_TEST(res != 0);
//
//										 auto result = co_await sql_conn.async_select<personal>(sql_select(personal)());
//
//										 BOOST_TEST(!result.empty());
//									 },aquarius::use_future);
//
//
//	io.run();
//
//	future.get();
//}

BOOST_AUTO_TEST_SUITE_END()