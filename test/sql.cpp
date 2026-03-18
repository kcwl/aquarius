#define BOOST_TEST_NO_MAIN
#include <aquarius.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(sql)

using namespace aquarius::tbl;
using namespace aquarius;

struct personal
{
	fields<int64> age;
	fields<int64> sex;

	constexpr static auto member()
	{
		return std::make_tuple(
			&personal::age,
			&personal::sex
		);
	}

	constexpr static auto member_name()
	{
		return std::make_tuple(
			"age"sv,
			"sex"sv
		);
	}
};



BOOST_AUTO_TEST_CASE(sql_)
{
	personal p{ 1, true };

	auto sql1 = aquarius::tbl::insert(p);

	BOOST_TEST(sql1 == "insert into personal(age,sex) values(1,1)");

	auto sql3 = aquarius::tbl::remove(p);

	BOOST_TEST(sql3 == "delete from personal");

	p.age.set_condition(1);
	p.sex.set_condition(1);

	auto sql2 = aquarius::tbl::update(p);

	BOOST_TEST(sql2 == "update personal set age=1 and sex=1");

	p.age.clear();
	p.sex.clear();
	auto sql = aquarius::tbl::select<personal>(p);

	BOOST_TEST(sql == "select * from personal");

	// auto sql4 = create_view<test_model>();

	// BOOST_TEST(sql4() == "create table if not exists test_model(id1 int1 primary key,id2 int2, id3 int3, id4 int4,
	// id5 int8, id6 float, id7 double)");
}

using namespace std::chrono_literals;

BOOST_AUTO_TEST_CASE(connecting)
{
	personal p{ 1, 1 };

	aquarius::io_context io;

	boost::mysql::pool_params params{};
	params.server_address.emplace_host_and_port("localhost");
	params.username = "root";
	params.database = "unittest";
	params.ssl = boost::mysql::ssl_mode::disable;

	aquarius::tbl::mysql connector(io, std::move(params));

	boost::mysql::any_connection conn(io);

	std::this_thread::sleep_for(2s);

	aquarius::error_code ec{};

	auto fur = aquarius::co_spawn(
		io,
		[&] -> aquarius::awaitable<void>
		{
			try
			{
				connector.async_run();

				auto res = co_await connector.async_execute("insert into personal values(1,1)", ec);

				BOOST_TEST(res != 0);

				auto select_res = co_await connector.async_query<personal>("select * from personal", ec);

				BOOST_TEST(select_res.size() != 0);
			}
			catch (const boost::mysql::error_with_diagnostics& err)
			{
				std::cerr << "Error: " << err.what() << ", error code: " << err.code() << '\n'
					<< "Server diagnostics: " << err.get_diagnostics().server_message() << std::endl;
			}

		},
		aquarius::use_future);

	std::thread t([&] { io.run(); });

	fur.get();

	io.stop();

	t.join();
}

BOOST_AUTO_TEST_SUITE_END()
