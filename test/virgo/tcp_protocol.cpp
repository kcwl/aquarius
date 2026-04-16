#define BOOST_TEST_NO_MAIN
#include <aquarius.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

struct mock_header
{
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
	error_code serialize(flex_buffer&)
	{
		return error_code{};
	}

	error_code deserialize(flex_buffer&)
	{
		return error_code{};
	}
};

BOOST_AUTO_TEST_SUITE(ut_tcp_protocol)

BOOST_AUTO_TEST_CASE(commit)
{
	using tcp_request = basic_tcp_protocol<true, mock_header, mock_body>;

	tcp_request req{};

	flex_buffer buffer{};

	BOOST_CHECK_EQUAL(req.commit(buffer), error_code{});

	BOOST_TEST(buffer.size() = 4);

	using tcp_response = basic_tcp_protocol<false, mock_header, mock_body>;

	tcp_response resp{};

	BOOST_CHECK_EQUAL(resp.commit(buffer), error_code{});

	BOOST_TEST(buffer.size() = 4);
}

BOOST_AUTO_TEST_CASE(consume)
{
	using tcp_request = basic_tcp_protocol<true, mock_header, mock_body>;

	tcp_request req{};

	flex_buffer buffer{};

	BOOST_TEST(req.consume(buffer));

	BOOST_TEST(buffer.size() == 0);

	using tcp_response = basic_tcp_protocol<false, mock_header, mock_body>;

	tcp_response resp{};

	BOOST_TEST(resp.consume(buffer));

	BOOST_TEST(buffer.size() == 0);
}

BOOST_AUTO_TEST_CASE(response_result)
{
	using tcp_response = basic_tcp_protocol<false, mock_header, mock_body>;

	tcp_response resp{};

	BOOST_CHECK_EQUAL(resp.result(), 0);

	resp.result() = 1;

	BOOST_CHECK_EQUAL(resp.result(), 1);

	resp.result() = 2;

	BOOST_CHECK_EQUAL(resp.result(), 2);
}

BOOST_AUTO_TEST_SUITE_END()