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

    using BaseResp = aquarius::tcp_response<"10002", dummy_body>;

    struct resp_t : public BaseResp
    {
        using Base = BaseResp;

        virtual aquarius::error_code commit(aquarius::flex_buffer& buffer) override
        {
            // serialize header and body into a temp buffer
            boost::asio::streambuf tmp_body;
            this->header().serialize(tmp_body);
            this->body().serialize(tmp_body);

            // serialize result varint into a temp buffer
            boost::asio::streambuf tmp_res;
            aquarius::binary_parse().to_datas(this->result(), tmp_res);

            // copy tmp_res + tmp_body into output buffer
            for (auto it = boost::asio::buffer_sequence_begin(tmp_res.data()); it != boost::asio::buffer_sequence_end(tmp_res.data()); ++it)
            {
                auto b = *it;
                buffer.sputn(static_cast<const char*>(b.data()), b.size());
            }

            for (auto it = boost::asio::buffer_sequence_begin(tmp_body.data()); it != boost::asio::buffer_sequence_end(tmp_body.data()); ++it)
            {
                auto b = *it;
                buffer.sputn(static_cast<const char*>(b.data()), b.size());
            }

            return aquarius::error_code{};
        }

        virtual aquarius::error_code consume(aquarius::flex_buffer& buffer) override
        {
                // copy full buffer into a temporary contiguous string
                std::string whole;
                whole.reserve(buffer.size());

                auto bufs = buffer.data();
                auto sb = boost::asio::buffer_sequence_begin(bufs);
                auto se = boost::asio::buffer_sequence_end(bufs);
                for (auto it = sb; it != se; ++it)
                {
                    auto b = *it;
                    const char* p = static_cast<const char*>(b.data());
                    whole.append(p, b.size());
                }

                // parse result using a temp streambuf so parsing does not affect original
                boost::asio::streambuf temp_all;
                temp_all.sputn(whole.data(), whole.size());

                try
                {
                    int32_t val = aquarius::binary_parse().from_datas<int32_t>(temp_all);
                    this->result() = val;
                }
                catch (...)
                {
                    return aquarius::ip::error::consume;
                }

                // determine how many bytes were consumed from temp_all
                std::size_t consumed = whole.size() - temp_all.size();

                // build a buffer containing only the header+body (after consumed bytes)
                boost::asio::streambuf tmp2;
                if (consumed < whole.size())
                {
                    tmp2.sputn(whole.data() + consumed, whole.size() - consumed);
                }

                // deserialize header and body directly from tmp2
                try
                {
                    this->header().deserialize(tmp2);
                    this->body().deserialize(tmp2);
                }
                catch (...)
                {
                    return aquarius::ip::error::consume;
                }

                return aquarius::error_code{};
        }
    };

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

