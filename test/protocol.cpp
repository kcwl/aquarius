#define BOOST_TEST_NO_MAIN
#include <aquarius/basic_protocol.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

struct mock_header
{
	int a;

	error_code serialize(flex_buffer&)
	{
		return error_code{};
	}
	error_code deserialize(flex_buffer&)
	{
		return error_code{};
	}
};

struct mock_body
{
	int b;

	error_code serialize(flex_buffer&)
	{
		return error_code{};
	}
	error_code deserialize(flex_buffer&)
	{
		return error_code{};
	}
};

BOOST_AUTO_TEST_SUITE(ut_basic_protocol)

BOOST_AUTO_TEST_CASE(ctor)
{
	using protocol = basic_protocol<mock_header, mock_body*, std::allocator<mock_body>>;

	protocol pt{};
	pt.header().a = 1;
	pt.body().b = 2;

	auto pt1 = std::move(pt);

	BOOST_CHECK_EQUAL(pt1.header().a, 1);
	BOOST_CHECK_EQUAL(pt1.body().b, 2);
	BOOST_CHECK_EQUAL(pt1.method(), 0);

	auto pt2(std::move(pt1));
	BOOST_CHECK_EQUAL(pt2.header().a, 1);
	BOOST_CHECK_EQUAL(pt2.body().b, 2);
	BOOST_CHECK_EQUAL(pt2.method(), 0);
}

BOOST_AUTO_TEST_CASE(method)
{
	using protocol = basic_protocol<mock_header, mock_body*, std::allocator<mock_body>>;

	protocol pt{};
	pt.method(1);

	BOOST_CHECK_EQUAL(pt.method(), 1);

	pt.method() = 2;

	BOOST_CHECK_EQUAL(pt.method(), 2);
}

BOOST_AUTO_TEST_CASE(commit_and_consume)
{
	using protocol = basic_protocol<mock_header, mock_body*, std::allocator<mock_body>>;

	protocol pt{};

	flex_buffer buffer{};

	BOOST_TEST(!pt.commit(buffer));
	BOOST_TEST(pt.consume(buffer));
}

BOOST_AUTO_TEST_SUITE_END()