#define BOOST_TEST_NO_MAIN
#include <aquarius.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(sql)

#ifdef ENABLE_MYSQL
struct personal
{
	int age;
	bool sex;
};

using namespace aquarius::tbl;

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

	// auto sql4 = create_view<test_model>();

	// BOOST_TEST(sql4() == "create table if not exists test_model(id1 int1 primary key,id2 int2, id3 int3, id4 int4,
	// id5 int8, id6 float, id7 double)");
}

using namespace std::chrono_literals;

BOOST_AUTO_TEST_CASE(connecting)
{
	personal p{ 1, true };

	aquarius::io_service_pool pool(1);

	using sql_pool = aquarius::basic_pool<aquarius::sql_connector>;

	sql_pool _sql_pool{};
	
	aquarius::database_param db_param{};
	db_param.host = "localhost";
	db_param.user = "root";
	db_param.db = "unittest";

	auto future = aquarius::co_spawn(pool.get_io_service(), _sql_pool.run(1, db_param),
									 aquarius::use_future);

	std::thread t([&] { pool.run(); });

	auto result = future.get();

	BOOST_TEST(result);

	auto fur = aquarius::co_spawn(
		pool.get_io_service(),
		[&] -> aquarius::awaitable<void>
		{
			// auto res = co_await aquarius::tbl::schedule_create<std::size_t>("sql", test_model{});

			// BOOST_TEST(res != 0);

			//auto res = co_await aquarius::tbl::schedule_insert<std::size_t>("sqlmodule", p);
			auto insert_task = std::make_shared<sql_task<insert, personal, std::size_t>>(std::move(p));

			auto res = co_await (*insert_task)(_sql_pool.get_task_proc());

			BOOST_TEST(res != 0);

			auto select_task = std::make_shared<sql_task<select_, personal, std::vector<personal>>>();
			auto select_res = co_await (*select_task)(_sql_pool.get_task_proc());

			BOOST_TEST(select_res.size() != 0);
		},
		aquarius::use_future);

	fur.get();

	pool.stop();

	t.join();
}
#endif

BOOST_AUTO_TEST_SUITE_END()