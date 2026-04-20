#define BOOST_TEST_NO_MAIN
#include <aquarius/tbl/grep_view.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

struct mock_tbl
{
	int a;
	std::string b;
};

BOOST_AUTO_TEST_SUITE(ut_grep_view)

BOOST_AUTO_TEST_CASE(less)
{
	auto result = std::move(grep<&mock_tbl::a>() < 3);

	BOOST_CHECK_EQUAL(static_cast<std::string>(result), "a < 3");
}

BOOST_AUTO_TEST_CASE(less_euqal)
{
	auto result = std::move(grep<&mock_tbl::a>() <= 3);

	BOOST_CHECK_EQUAL(static_cast<std::string>(result), "a <= 3");
}

BOOST_AUTO_TEST_CASE(great)
{
	auto result = std::move(grep<&mock_tbl::a>() > 3);

	BOOST_CHECK_EQUAL(static_cast<std::string>(result), "a > 3");
}

BOOST_AUTO_TEST_CASE(great_equal)
{
	auto result = std::move(grep<&mock_tbl::a>() >= 3);

	BOOST_CHECK_EQUAL(static_cast<std::string>(result), "a >= 3");
}

BOOST_AUTO_TEST_CASE(equal)
{
	auto result = std::move(grep<&mock_tbl::a>() == 3);

	BOOST_CHECK_EQUAL(static_cast<std::string>(result), "a = 3");
}

BOOST_AUTO_TEST_CASE(_or_)
{
	auto result = std::move(grep<&mock_tbl::a>() >= 3 || grep<&mock_tbl::a>() <= 1);

	BOOST_CHECK_EQUAL(static_cast<std::string>(result), "(a >= 3 or a <= 1)");
}

BOOST_AUTO_TEST_SUITE_END()