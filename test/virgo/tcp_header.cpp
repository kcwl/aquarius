#define BOOST_TEST_NO_MAIN
#include <aquarius.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;



BOOST_AUTO_TEST_SUITE(ut_tcp_header)

BOOST_AUTO_TEST_CASE(ctor)
{
    tcp_header header{};

    header.uuid(1);
    header.timestamp(2);

    tcp_header h1 = std::move(header);

    BOOST_CHECK_EQUAL(h1.uuid(), 1);
    BOOST_CHECK_EQUAL(h1.timestamp(), 2);

    tcp_header h2(std::move(h1));

    BOOST_CHECK_EQUAL(h2.uuid(), 1);
    BOOST_CHECK_EQUAL(h2.timestamp(), 2);
}

BOOST_AUTO_TEST_CASE(member_func)
{
    tcp_header header{};
    BOOST_CHECK_EQUAL(header.uuid(), 0);
    BOOST_CHECK_EQUAL(header.timestamp(), 0);

    header.uuid() = 1;
    header.timestamp() = 2;

    BOOST_CHECK_EQUAL(header.uuid(), 1);
    BOOST_CHECK_EQUAL(header.timestamp(), 2);
}

BOOST_AUTO_TEST_CASE(serialize_and_deserialize)
{
    tcp_header header{};
    flex_buffer buffer{};

    header.uuid(1);
    header.timestamp(2);

    BOOST_TEST(header.serialize(buffer) == error_code{});

    tcp_header h1{};
    BOOST_TEST(h1.deserialize(buffer) == error_code{});

    BOOST_TEST(h1.uuid() == header.uuid());
    BOOST_TEST(h1.timestamp() == header.timestamp());
}

BOOST_AUTO_TEST_SUITE_END()