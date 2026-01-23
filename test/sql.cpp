#define BOOST_TEST_NO_MAIN
#include <aquarius.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(sql)

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

	aquarius::io_context io;

	boost::mysql::pool_params params{};
	params.server_address.emplace_host_and_port("localhost", 3308);
	params.username = "root";
	params.database = "unittest";

	aquarius::tbl::mysql connector(io, std::move(params));

	connector.async_run();

	std::this_thread::sleep_for(2s);


	auto fur = aquarius::co_spawn(
		io,
		[&] -> aquarius::awaitable<void>
		{
			auto res = co_await aquarius::mpc_execute("insert into personal values(1,1)");

			BOOST_TEST(res != 0);

			auto select_res = co_await aquarius::mpc_query<personal>("select * from personal");

			BOOST_TEST(select_res.size() != 0);
		},
		aquarius::use_future);

	fur.get();
}

BOOST_AUTO_TEST_SUITE_END()