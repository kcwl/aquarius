#define BOOST_TEST_NO_MAIN
#include <aquarius.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

struct mock_tbl
{
	int a;
	std::string b;
};

BOOST_AUTO_TEST_SUITE(ut_update_view)

BOOST_AUTO_TEST_CASE(update)
{
	mock_tbl tbl{};
	tbl.a = 3;
	tbl.b = "test";
	auto& result = update_v(tbl);

	BOOST_CHECK_EQUAL(static_cast<std::string>(result), "update mock_select set a = 3 and b = \"test\"");
}

BOOST_AUTO_TEST_CASE(upate_condition)
{
	mock_tbl tbl{};
	tbl.a = 3;
	tbl.b = "test";

	auto& result = update_v(tbl) | grep<&mock_tbl::a> > 3;

	BOOST_CHECK_EQUAL(static_cast<std::string>(result), "update mock_tbl set a = 3 and b = \"test\" where a > 3");

	auto& res = update_v(tbl) | grep<&mock_tbl::a> > 3 | (grep<&mock_tbl::b> == "test" || grep<&mock_tbl::b> == "test1");

	BOOST_CHECK_EQUAL(static_cast<std::string>(res),
					  "update mock_tbl set a = 3 and b = \"test\" where a > 3 and (b = \"test\" or b = \"test1\")");
}

BOOST_AUTO_TEST_SUITE_END()