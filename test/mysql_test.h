#pragma once
#include <aquarius/mysql.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(mysql)

BOOST_AUTO_TEST_CASE(connect)
{
	aquarius::io_service_pool io_pool{ 5 };

	aquarius::service_pool<aquarius::mysql_connect> pool(io_pool, 3 ,"172.26.4.15", boost::mysql::default_port_string,
														 "kcwl", "123456", "test_mysql");

	pool.start();

	pool.stop();
}

BOOST_AUTO_TEST_SUITE_END()