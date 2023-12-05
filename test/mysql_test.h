#pragma once
#include <aquarius/mysql.hpp>
#include <chrono>
#include <boost/test/unit_test_suite.hpp>

using namespace std::chrono_literals;

BOOST_AUTO_TEST_SUITE(mysql)

struct person
{
	int age;
	std::string name;
};


BOOST_AUTO_TEST_CASE(connect)
{
	//aquarius::io_service_pool io_pool{ 5 };

	//aquarius::service_pool<aquarius::mysql_connect> pool(io_pool, "172.26.4.15", boost::mysql::default_port_string,
	//													 "kcwl", "123456", "test_mysql");

	//std::thread t([&] { io_pool.run(); });
	//
	//std::this_thread::sleep_for(3s);

	//pool.stop();

	//io_pool.stop();
	//t.join();
}

BOOST_AUTO_TEST_CASE(sql)
{
	aquarius::io_service_pool io_pool{ 5 };

	aquarius::service_pool<aquarius::mysql_connect> pool(io_pool, "172.26.4.15", boost::mysql::default_port_string,
														 "kcwl", "123456", "test_mysql");

	using mysql_sql = aquarius::chain_sql<aquarius::mysql_connect>;

	{
		BOOST_CHECK(mysql_sql(pool).select<person>().sql() ==
					"select * from person;");
	}

	{
		BOOST_CHECK(mysql_sql(pool).insert(person{ 1, "peter" }).sql() ==
					"insert into person values(1,'peter');");
	}
	
	{
		BOOST_CHECK(mysql_sql(pool).remove<person>().sql() == "delete from person;");
	}

	{
		BOOST_CHECK(mysql_sql(pool).update(person{ 1, "candy" }).sql() ==
					"update person set age = 1 and name = 'candy';");
	}
	
	{
		BOOST_CHECK(mysql_sql(pool).replace(person{ 1, "ridy" }).sql() == "replace into person values(1,'ridy');");
	}
}

BOOST_AUTO_TEST_SUITE_END()