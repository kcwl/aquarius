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

using namespace aquarius::tbl;

BOOST_AUTO_TEST_SUITE(sql)

BOOST_AUTO_TEST_CASE(sql_)
{
	personal p{ 1, true };

	auto sql1 = insert_view(p);

	BOOST_TEST(sql1() == "insert into personal values(1,1)");

	auto sql3 = remove_view<personal>();

	BOOST_TEST(sql3() == "delete from personal");

	auto sql2 = update_view(p);

	BOOST_TEST(sql2() == "update personal set age=1 and sex=1");

	constexpr auto sql = select_view<personal>();

	static_assert(sql() == "select * from personal");
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