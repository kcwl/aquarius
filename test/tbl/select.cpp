#define BOOST_TEST_NO_MAIN
#include <aquarius/tbl/grep_view.hpp>
#include <aquarius/tbl/select_view.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

struct mock_tbl
{
	int a;
	std::string b;
};

BOOST_AUTO_TEST_SUITE(ut_select_view)

BOOST_AUTO_TEST_CASE(select)
{
	auto& result = select_v<mock_tbl, &mock_tbl::b>();

	BOOST_CHECK_EQUAL(static_cast<std::string>(result), "select b from mock_tbl");

	auto& res = select_v<mock_tbl>();

	BOOST_CHECK_EQUAL(static_cast<std::string>(res), "select * from mock_tbl");
}

BOOST_AUTO_TEST_CASE(select_and)
{
	auto& result = select_v<mock_tbl, &mock_tbl::b>() | grep<&mock_tbl::a>() > 3;

	BOOST_CHECK_EQUAL(static_cast<std::string>(result), "select b from mock_tbl where a > 3");
}

BOOST_AUTO_TEST_CASE(select_and_or)
{
	auto& result = select_v<mock_tbl, &mock_tbl::b>() | grep<&mock_tbl::a>() > 3 |
				   (grep<&mock_tbl::b>() == "test" || grep<&mock_tbl::b>() == "test1");

	BOOST_CHECK_EQUAL(static_cast<std::string>(result),
					  "select b from mock_tbl where a > 3 and (b = \"test\" or b = \"test1\")");
}

BOOST_AUTO_TEST_SUITE_END()