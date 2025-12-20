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

struct mysql_config
{
	std::string host;
	std::string user;
	std::string db;
	std::string db_passwd;
};

class sqlmodule : public aquarius::_module<aquarius::sql_connector, mysql_config, "mysql.ini">
{
	using sql_pool = aquarius::basic_pool<aquarius::sql_connector>;

	using base_type = aquarius::_module<aquarius::sql_connector, mysql_config, "mysql.ini">;

public:
	using typename base_type::core_type;

	using typename base_type::config_type;

	using base_type::config_path;

public:
	virtual bool init() override
	{
		param_.host = configs().host;
		param_.user = configs().user;
		param_.db = configs().db;
		param_.password = configs().db_passwd;
	}

	virtual auto run() -> aquarius::awaitable<void>
	{
		co_await pool_.run(3, param_);
	}

	virtual std::shared_ptr<core_type> core() override
	{
		return pool_.get_task_proc();
	}

private:
	sql_pool pool_;

	aquarius::database_param param_;
};

AQUARIUS_MODULE(sqlmodule);

BOOST_AUTO_TEST_CASE(connecting)
{
	personal p{ 1, true };

	aquarius::io_service_pool pool(1);

	auto future = aquarius::co_spawn(pool.get_io_service(), aquarius::module_router::get_mutable_instance().run(),
									 aquarius::use_future);

	std::thread t([&] { pool.run(); });

	future.get();

	auto fur = aquarius::co_spawn(
		pool.get_io_service(),
		[&] -> aquarius::awaitable<void>
		{
			// auto res = co_await aquarius::tbl::schedule_create<std::size_t>("sql", test_model{});

			// BOOST_TEST(res != 0);

			auto res = co_await aquarius::tbl::schedule_insert<std::size_t>("sqlmodule", p);

			BOOST_TEST(res != 0);

			auto result = co_await aquarius::tbl::schedule_select<std::vector<personal>>("sqlmodule");

			BOOST_TEST(result.size() != 0);
		},
		aquarius::use_future);

	fur.get();

	pool.stop();

	t.join();
}
#endif

BOOST_AUTO_TEST_SUITE_END()