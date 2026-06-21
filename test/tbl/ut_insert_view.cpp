#include <aquarius/tbl/insert_view.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(ut_tbl_insert_view)
struct Person
{
	std::string name;
	int age;
};

BOOST_AUTO_TEST_CASE(basic_insert_sql)
{
	Person p{ "alice", 30 };

	auto sql = static_cast<std::string>(aquarius::insert_v(p));

	BOOST_TEST(sql.find("insert into") != std::string::npos);
	BOOST_TEST(sql.find("alice") != std::string::npos);
	BOOST_TEST(sql.find("30") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
