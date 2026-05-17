#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/tcp_response.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/serialize/binary.hpp>

using namespace aquarius;

struct dummy_body
{
    int x = 7;

    error_code serialize(flex_buffer& b) const
    {
        binary_parse().to_datas(x, b);
        return {};
    }

    error_code deserialize(flex_buffer& b)
    {
        x = binary_parse().from_datas<int>(b);
        return {};
    }
};

using BaseResp = aquarius::tcp_response<"rt", dummy_body>;

struct my_resp : public BaseResp
{
    using Base = BaseResp;

    virtual aquarius::error_code commit(aquarius::flex_buffer& buf) override
    {
        int32_t v = static_cast<int32_t>(this->result());
        buf.sputn(reinterpret_cast<char*>(&v), sizeof(v));
        return Base::commit(buf);
    }
};

BOOST_AUTO_TEST_SUITE(ut_virgo_tcp_response_commit)

BOOST_AUTO_TEST_CASE(commit_writes_result)
{
    my_resp r;

    r.result(12345);

    aquarius::flex_buffer buf{};

    BOOST_TEST(r.commit(buf) == error_code{});

    // read fixed 4-byte result written by the shim
    int32_t val{};
    auto n = buf.sgetn(reinterpret_cast<char*>(&val), sizeof(val));
    (void)n;
    BOOST_TEST(val == 12345);
}

BOOST_AUTO_TEST_SUITE_END()
