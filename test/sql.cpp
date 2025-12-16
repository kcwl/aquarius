#define BOOST_TEST_NO_MAIN
#include <aquarius/asio.hpp>
#include <aquarius/io_service_pool.hpp>
#include <aquarius/tbl.hpp>
#include <boost/test/unit_test.hpp>
#include "test_model.tbl.h"

struct personal
{
	int age;
	bool sex;
};

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

using namespace std::chrono_literals;

#if defined(MYSQL_SQL)
BOOST_AUTO_TEST_CASE(connecting)
{
	personal p{ 1, true };

	aquarius::io_service_pool pool(1);

	aquarius::database_param db_param{};
	db_param.host = "localhost";
	db_param.user = "kcwl";
	db_param.password = "NN0705lwl1217&";
	db_param.db = "unittest";

	auto future = aquarius::co_spawn(pool.get_io_service(), aquarius::sql_pool().run(pool.size(), db_param), aquarius::use_future);

	std::thread t([&] { pool.run(); });

	future.get();

	auto fur = aquarius::co_spawn(
		pool.get_io_service(),
		[&] -> aquarius::awaitable<void>
		{
			auto res = co_await aquarius::sql_pool().async_execute(aquarius::make_execute_task(sql_insert(p)()));

			BOOST_TEST(res != 0);

			auto result = co_await aquarius::sql_pool().async_execute(
				aquarius::make_query_task<personal>(sql_select(personal)()));

			BOOST_TEST(result.size() != 0);
		},
		aquarius::use_future);

	fur.get();

	pool.stop();

	t.join();
}
#endif

BOOST_AUTO_TEST_SUITE_END()