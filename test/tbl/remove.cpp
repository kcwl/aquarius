#define BOOST_TEST_NO_MAIN
#include <aquarius.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

struct mock_tbl
{
	int a;
	std::string b;
};

BOOST_AUTO_TEST_SUITE(ut_remove_view)

BOOST_AUTO_TEST_CASE(remove)
{
	auto& result = remove_v<mock_tbl>();

	BOOST_CHECK_EQUAL(static_cast<std::string>(result), "delete from mock_tbl");
}

BOOST_AUTO_TEST_CASE(remove_condition)
{
	mock_tbl tbl{};
	tbl.a = 3;
	tbl.b = "test";

	auto& result = remove_v<mock_tbl>() | grep<&mock_tbl::a> > 3;

	BOOST_CHECK_EQUAL(static_cast<std::string>(result), "delete from mock_tbl where a > 3");

	auto& res =
		remove_v<mock_tbl>() | grep<&mock_tbl::a>() > 3 | (grep<&mock_tbl::b>() == "test" || grep<&mock_tbl::b>() == "test1");

	BOOST_CHECK_EQUAL(static_cast<std::string>(res),
					  "delete from mock_tbl where a > 3 and (b = \"test\" or b = \"test1\")");
}

BOOST_AUTO_TEST_SUITE_END()