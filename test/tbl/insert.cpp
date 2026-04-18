#define BOOST_TEST_NO_MAIN
#include <aquarius/tbl/insert_view.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

struct mock_tbl
{
	int a;
	std::string b;
};

BOOST_AUTO_TEST_SUITE(ut_insert_view)

BOOST_AUTO_TEST_CASE(insert)
{
	mock_tbl tbl{};
	tbl.a = 3;
	tbl.b = "test";

	auto& result = insert_v(tbl);

	BOOST_CHECK_EQUAL(static_cast<std::string>(result), "insert into mock_tbl(a,b) values(3,\"test\")");
}

BOOST_AUTO_TEST_SUITE_END()