#pragma once
#include <aquarius/mysql.hpp>
#include <chrono>
#include <boost/test/unit_test_suite.hpp>

using namespace std::chrono_literals;

BOOST_AUTO_TEST_SUITE(mysql)

BOOST_AUTO_TEST_CASE(connect)
{
	aquarius::io_service_pool io_pool{ 5 };

	aquarius::service_pool<aquarius::mysql_connect> pool(io_pool, "172.26.4.15", boost::mysql::default_port_string,
														 "kcwl", "123456", "test_mysql");

	std::thread t([&] { io_pool.run(); });
	
	std::this_thread::sleep_for(3s);

	pool.stop();

	io_pool.stop();
	t.join();
}

struct person
{
	int age;
	std::string name;
};

BOOST_AUTO_TEST_CASE(sql)
{ 
	aquarius::io_service_pool io_pool{ 5 };

	std::vector<person> result = chain_sql(io_pool).select<person>().where(AQUARIUS_ATTRIBUTE(person, age) != 1).async_query();
}

BOOST_AUTO_TEST_SUITE_END()