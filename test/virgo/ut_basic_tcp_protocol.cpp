#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/basic_tcp_protocol.hpp>
#include <aquarius/virgo/tcp_header.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_basic_tcp_protocol)

struct mock_header
{
    error_code serialize(flex_buffer&)
    {
        return error_code{};
    }

    error_code deserialize(flex_buffer&)
    {
        uuid = 1;
        return error_code{};
    }

    int uuid;
};

struct mock_body
{
    error_code serialize(flex_buffer&) { return error_code{}; }

    error_code deserialize(flex_buffer&)
    {
        value = 2;
        return error_code{};
    }

    int value;
};

struct mock_failed_header
{
    error_code serialize(flex_buffer&)
    {
        throw std::runtime_error("failed header serialize");
    }

    error_code deserialize(flex_buffer&)
    {
        throw std::runtime_error("failed header deserialize");
    }

    int uuid;
};

struct mock_failed_body
{
    error_code serialize(flex_buffer&) { throw; }

    error_code deserialize(flex_buffer&)
    {
        throw;
    }

    int value;
};

BOOST_AUTO_TEST_CASE(tcp_commit_failed)
{
    basic_tcp_protocol<true, "", mock_failed_header, mock_failed_body> proto{};

    flex_buffer buffer{};
    auto ec = proto.commit(buffer);

    BOOST_TEST(ec == ip::error::commit);
}

BOOST_AUTO_TEST_CASE(tcp_commit_success)
{
    basic_tcp_protocol<true, "", mock_header, mock_body> proto{};

    flex_buffer buffer{};
    auto ec = proto.commit(buffer);

    BOOST_TEST(!ec);
}

BOOST_AUTO_TEST_CASE(tcp_consume_failed)
{
    basic_tcp_protocol<true, "", mock_failed_header, mock_failed_body> proto{};

    flex_buffer buffer{};
    auto ec = proto.consume(buffer);

    BOOST_TEST(ec == ip::error::consume);
}

BOOST_AUTO_TEST_CASE(tcp_consume_success)
{
    basic_tcp_protocol<true, "", mock_header, mock_body> proto{};

    flex_buffer buffer{};
    auto ec = proto.consume(buffer);

    BOOST_TEST(!ec);
}

BOOST_AUTO_TEST_SUITE_END()
