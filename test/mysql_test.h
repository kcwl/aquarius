#pragma once
#include <aquarius/mysql.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <chrono>

using namespace std::chrono_literals;

using namespace std::string_view_literals;

BOOST_AUTO_TEST_SUITE(mysql)

struct products
{
	int prod_id;
	std::string prod_name;
	int prod_price;
	int vend_id;
};

BOOST_AUTO_TEST_CASE(connect)
{
	// aquarius::io_service_pool io_pool{ 5 };

	// aquarius::service_pool<aquarius::mysql_connect> pool(io_pool, "172.26.4.15", boost::mysql::default_port_string,
	//													 "kcwl", "123456", "test_mysql");

	// std::thread t([&] { io_pool.run(); });
	//
	// std::this_thread::sleep_for(3s);

	// pool.stop();

	// io_pool.stop();
	// t.join();
}

BOOST_AUTO_TEST_CASE(sql)
{
	aquarius::io_service_pool io_pool{ 5 };

	aquarius::service_pool<aquarius::mysql_connect> pool(io_pool, "172.26.4.15", boost::mysql::default_port_string,
														 "kcwl", "123456", "test_mysql");

	{
		using mysql_sql = aquarius::select_chain<aquarius::mysql_connect>;

		auto sql = mysql_sql(pool).select<products, AQUARIUS_SQL_BIND(prod_name)>().sql();

		BOOST_CHECK_EQUAL(sql, "select prod_name from products");

		sql = mysql_sql(pool).select<products, AQUARIUS_SQL_BIND(prod_id, prod_name, prod_price)>().sql();

		BOOST_CHECK_EQUAL(sql, "select prod_id, prod_name, prod_price from products");

		BOOST_CHECK_EQUAL(mysql_sql(pool).select<products>().sql(), "select * from products");

		sql = mysql_sql(pool).select_distinct<products, AQUARIUS_SQL_BIND(vend_id)>().sql();
		BOOST_CHECK_EQUAL(sql, "select distinct vend_id from products");

		sql = mysql_sql(pool).select_top<products, 5, AQUARIUS_SQL_BIND(prod_name)>().sql();

		BOOST_CHECK_EQUAL(sql, "select top 5 prod_name from products");

		sql = mysql_sql(pool).select<products, AQUARIUS_SQL_BIND(prod_name)>().limit<5>().sql();
		BOOST_CHECK_EQUAL(sql, "select prod_name from products limit 5");

		sql = mysql_sql(pool).select<products, AQUARIUS_SQL_BIND(prod_name)>().limit<5>().offset<5>().sql();
		BOOST_CHECK_EQUAL(sql, "select prod_name from products limit 5 offset 5");

		sql = mysql_sql(pool)
				  .select<products, AQUARIUS_SQL_BIND(prod_name)>()
				  .order_by<AQUARIUS_SQL_BIND(prod_name)>()
				  .sql();
		BOOST_CHECK_EQUAL(sql, "select prod_name from products order by prod_name");

		sql = mysql_sql(pool)
				  .select<products, AQUARIUS_SQL_BIND(prod_id, prod_price, prod_name)>()
				  .order_by<AQUARIUS_SQL_BIND(prod_price, prod_name)>()
				  .sql();
		BOOST_CHECK_EQUAL(sql, "select prod_id, prod_price, prod_name from products order by prod_price, prod_name");

		sql = mysql_sql(pool)
				  .select<products, AQUARIUS_SQL_BIND(prod_id, prod_price, prod_name)>()
				  .order_by_index<2, 3>()
				  .sql();
		BOOST_CHECK_EQUAL(sql, "select prod_id, prod_price, prod_name from products order by 2, 3");

		sql =
			mysql_sql(pool).select<products, AQUARIUS_SQL_BIND(vend_id)>().group_by<AQUARIUS_SQL_BIND(vend_id)>().sql();
		BOOST_CHECK_EQUAL(sql, "select vend_id from products group by vend_id");

		sql = mysql_sql(pool)
				  .select<products, AQUARIUS_SQL_BIND(vend_id)>()
				  .group_by<AQUARIUS_SQL_BIND(vend_id)>()
				  .having(AQUARIUS_EXPR(vend_id) > 2)
				  .sql();
		BOOST_CHECK_EQUAL(sql, "select vend_id from products group by vend_id having vend_id > 2");

		sql = mysql_sql(pool)
				  .select<products, AQUARIUS_SQL_BIND(prod_name, prod_price)>()
				  .where(AQUARIUS_EXPR(prod_price) == 3.49)
				  .sql();

		BOOST_CHECK_EQUAL(sql, "select prod_name, prod_price from Products where prod_price = 3.49");

		sql = mysql_sql(pool)
				  .select<products, AQUARIUS_SQL_BIND(prod_name, prod_price)>()
				  .where(AQUARIUS_EXPR(prod_name) != "3.49" | AQUARIUS_EXPR(prod_name) <= "3.91")
				  .sql();

		BOOST_CHECK_EQUAL(
			sql, "select prod_name, prod_price from products where prod_name != '3.49' or prod_name <= '3.91'");
	}

	{
		using mysql_sql = aquarius::chain_sql<aquarius::mysql_connect>;

		BOOST_CHECK_EQUAL(mysql_sql(pool).insert(products{ 1, "peter", 2, 3 }).sql(),
						  "insert into products values(1,'peter',2,3)");
	}

	{
		using mysql_sql = aquarius::chain_sql<aquarius::mysql_connect>;

		BOOST_CHECK_EQUAL(mysql_sql(pool).remove<products>().sql(), "delete from products");
	}

	{
		using mysql_sql = aquarius::chain_sql<aquarius::mysql_connect>;

		BOOST_CHECK_EQUAL(mysql_sql(pool).update(products{ 1, "candy", 3, 5 }).sql(),
						  "update person set age = 1 and name = 'candy'");
	}

	{
		using mysql_sql = aquarius::chain_sql<aquarius::mysql_connect>;

		BOOST_CHECK_EQUAL(mysql_sql(pool).replace(products{ 1, "ridy", 6, 7 }).sql(),
						  "replace into products values(1,'ridy',6,7)");
	}
}

BOOST_AUTO_TEST_SUITE_END()