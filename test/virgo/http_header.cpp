#define BOOST_TEST_NO_MAIN
#include <aquarius/virgo/http_header.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_http_header)

BOOST_AUTO_TEST_CASE(ctor)
{
	http_header header{};

	header.set_field("Content-Length", "3");

	http_header h1 = std::move(header);

	BOOST_CHECK_EQUAL(h1.find("Content-Length"), "3");

	http_header h2(std::move(h1));

	BOOST_CHECK_EQUAL(h2.find("Content-Length"), "3");
}

BOOST_AUTO_TEST_CASE(member_func)
{
	http_header header{};
	BOOST_CHECK_EQUAL(header.keep_alive(), false);

	header.keep_alive(true);

	BOOST_CHECK_EQUAL(header.keep_alive(), true);

	BOOST_CHECK_EQUAL(header.content_type(), "");

	header.content_type("json");

	BOOST_CHECK_EQUAL(header.content_type(), "json");

	BOOST_CHECK_EQUAL(header.content_length(), 0);

	header.content_type("json");

	BOOST_CHECK_EQUAL(header.content_length(), 0);
}

BOOST_AUTO_TEST_CASE(serialize_and_deserialize)
{
	http_header header{};
	flex_buffer buffer{};

	header.content_type("json");
	header.content_length(2);

	BOOST_TEST(header.serialize(buffer) == error_code{});

	http_header h1{};
	BOOST_TEST(h1.deserialize(buffer) == error_code{});

	BOOST_TEST(h1.content_type() == header.content_type());
	BOOST_TEST(h1.content_length() == header.content_length());
}

BOOST_AUTO_TEST_CASE(serialize_error)
{
	std::string content = "Content-Length=1";
	flex_buffer buffer{};

	buffer.sputn(content.c_str(), content.size());

	http_header header{};

	BOOST_TEST(header.deserialize(buffer) == asio::error::eof);
}

BOOST_AUTO_TEST_SUITE_END()
