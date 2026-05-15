#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/tcp_response.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/serialize/tcp_serialize.hpp>
#include <aquarius/detail/flex_buffer.hpp>

using namespace aquarius;

struct dummy_body : public aquarius::tcp_serialize
{
    int x = 123;
    virtual void serialize(flex_buffer& buffer) override
    {
        binary_parse bp;
        bp.to_datas(x, buffer);
    }
    virtual void deserialize(flex_buffer& buffer) override
    {
        x = binary_parse().from_datas<int>(buffer);
    }
};

BOOST_AUTO_TEST_SUITE(ut_virgo_tcp_response)

BOOST_AUTO_TEST_CASE(commit_and_consume)
{
    aquarius::flex_buffer buffer{};

    using resp_t = aquarius::tcp_response<"10002", dummy_body>;

    resp_t resp;
    resp.result(42);
    resp.body().x = 7;

    BOOST_TEST(resp.commit(buffer) == error_code{});
    BOOST_TEST(buffer.size() > 0);

    resp_t other;
    auto ec = other.consume(buffer);
    BOOST_TEST(!ec);
    BOOST_TEST(other.result() == 42);
    BOOST_TEST(other.body().x == 7);
}

BOOST_AUTO_TEST_SUITE_END()

