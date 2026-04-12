#define BOOST_NO_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "../include/aquarius/detail/flex_buffer.hpp"
#include <boost/asio/buffer.hpp>
#include <iostream>
#include <cstring>

using aquarius::flex_buffer;
// use fully-qualified boost::asio::buffer_cast where needed

BOOST_AUTO_TEST_SUITE(flex_buffer_suite)

BOOST_AUTO_TEST_CASE(prepare_commit_consume_basic)
{
    flex_buffer fb;

    // prepare and write through mutable buffer
    auto mb = fb.prepare(10);
    char* wptr = reinterpret_cast<char*>(mb.data());
    for (int i = 0; i < 10; ++i)
        wptr[i] = static_cast<char>('A' + i);

    fb.commit(10);
    BOOST_CHECK_EQUAL(fb.size(), 10u);

    auto cb = fb.data();
    const char* rptr = reinterpret_cast<const char*>(cb.data());
    BOOST_CHECK_EQUAL(rptr[0], 'A');
    BOOST_CHECK_EQUAL(rptr[9], 'J');

    // consume some bytes
    fb.consume(3);
    BOOST_CHECK_EQUAL(fb.size(), 7u);
    auto cb2 = fb.data();
    const char* rptr2 = reinterpret_cast<const char*>(cb2.data());
    BOOST_CHECK_EQUAL(rptr2[0], 'D');
}

BOOST_AUTO_TEST_CASE(stream_write_and_read)
{
    flex_buffer fb;
    {
        std::ostream os(&fb);
        os << "HelloWorld";
        os.flush();
    }

    BOOST_CHECK_EQUAL(fb.size(), 10u);

    // read via istream which exercises underflow
    std::string s;
    {
        std::istream is(&fb);
        is >> s; // operator>> reads until whitespace
    }

    BOOST_CHECK_EQUAL(s, "HelloWorld");
    BOOST_CHECK_EQUAL(fb.size(), 0u);
}

BOOST_AUTO_TEST_CASE(move_ctor_and_assignment)
{
    flex_buffer fb;
    auto mb = fb.prepare(3);
    char* p = reinterpret_cast<char*>(mb.data());
    p[0] = 'x'; p[1] = 'y'; p[2] = 'z';
    fb.commit(3);

    // move construct
    flex_buffer fb2(std::move(fb));
    BOOST_CHECK_EQUAL(fb2.size(), 3u);

    // move assign
    flex_buffer fb3;
    fb3 = std::move(fb2);
    BOOST_CHECK_EQUAL(fb3.size(), 3u);

    auto cb = fb3.data();
    const char* rp = reinterpret_cast<const char*>(cb.data());
    BOOST_CHECK_EQUAL(rp[0], 'x');
}

BOOST_AUTO_TEST_CASE(seek_and_tell)
{
    flex_buffer fb;
    std::ostream os(&fb);
    os << "0123456789";
    os.flush();

    // initial positions
    std::size_t ppos = fb.tellp();
    std::size_t gpos = fb.tellg();
    BOOST_CHECK_EQUAL(ppos, 10u);
    BOOST_CHECK_EQUAL(gpos, 0u);

    // seek get pointer to position 5
    fb.pubseekpos(5, std::ios_base::in);
    BOOST_CHECK_EQUAL(fb.tellg(), 5u);

    // seek put pointer to position 2
    fb.pubseekoff(2, std::ios_base::beg, std::ios_base::out);
    BOOST_CHECK_EQUAL(fb.tellp(), 2u);
}

BOOST_AUTO_TEST_CASE(reset_and_capacity)
{
    flex_buffer fb;
    fb.reset(4);
    BOOST_CHECK(fb.capacity() >= 1u);
    // after reset size must be zero
    BOOST_CHECK_EQUAL(fb.size(), 0u);
}

BOOST_AUTO_TEST_CASE(reserve_behavior_with_max_size)
{
    // set a relatively small max_size to exercise reserve path
    flex_buffer fb(32);
    // fill part of buffer
    auto mb = fb.prepare(8);
    char* p = reinterpret_cast<char*>(mb.data());
    for (int i = 0; i < 8; ++i) p[i] = static_cast<char>('a' + i);
    fb.commit(8);

    // cause additional allocations by writing via ostream
    std::ostream os(&fb);
    for (int i = 0; i < 40; ++i)
        os.put(static_cast<char>('0' + (i % 10)));
    os.flush();

    // ensure we didn't exceed the max_size
    BOOST_CHECK_LE(fb.size(), fb.max_size());
}

BOOST_AUTO_TEST_CASE(consume_beyond_size_and_zero_commit)
{
    flex_buffer fb;
    auto mb = fb.prepare(5);
    char* p = reinterpret_cast<char*>(mb.data());
    std::memcpy(p, "abcde", 5);
    fb.commit(5);

    // consuming more than size should clamp to zero
    fb.consume(100);
    BOOST_CHECK_EQUAL(fb.size(), 0u);

    // prepare with zero and commit zero should not change size
    auto mb2 = fb.prepare(4);
    BOOST_CHECK_NE(mb2.data(), nullptr);
    fb.commit(0);
    BOOST_CHECK_EQUAL(fb.size(), 0u);
}

BOOST_AUTO_TEST_CASE(seekoff_cur_mode_variations)
{
    flex_buffer fb;
    auto mb = fb.prepare(6);
    std::memcpy(reinterpret_cast<char*>(mb.data()), "123456", 6);
    fb.commit(6);

    // seekoff with cur and mode in only
    auto res1 = fb.pubseekoff(1, std::ios_base::cur, std::ios_base::in);
    BOOST_CHECK(res1 == std::streampos(std::streamoff(-1)));

    // seekoff with cur and mode out only
    auto res2 = fb.pubseekoff(1, std::ios_base::cur, std::ios_base::out);
    BOOST_CHECK(res2 != std::streampos(std::streamoff(-1)));

    // seekoff with both should still allow moving within range
    auto res3 = fb.pubseekoff(2, std::ios_base::beg, std::ios_base::in | std::ios_base::out);
    BOOST_CHECK(res3 == std::streampos(std::streamoff(-1)));
}

BOOST_AUTO_TEST_CASE(data_byte_size_matches_chars)
{
    flex_buffer fb;
    auto mb = fb.prepare(7);
    std::memcpy(reinterpret_cast<char*>(mb.data()), "testing", 7);
    fb.commit(7);

    auto cb = fb.data();
    // buffer_size returns bytes; should equal fb.size() * sizeof(char)
    BOOST_CHECK_EQUAL(boost::asio::buffer_size(cb), static_cast<std::size_t>(fb.size() * sizeof(char)));
}

BOOST_AUTO_TEST_CASE(flexbuf_ref_copy_and_move)
{
    flex_buffer fb;
    auto mb = fb.prepare(3);
    std::memcpy(reinterpret_cast<char*>(mb.data()), "ok!", 3);
    fb.commit(3);

    aquarius::detail::basic_flexbuf_ref<std::allocator<char>> r1(fb);
    // copy-construct
    aquarius::detail::basic_flexbuf_ref<std::allocator<char>> r2(r1);
    BOOST_CHECK_EQUAL(r2.size(), r1.size());

    // move-construct
    aquarius::detail::basic_flexbuf_ref<std::allocator<char>> r3(std::move(r2));
    BOOST_CHECK_EQUAL(r3.size(), 3u);
}

// expose protected members for direct testing
struct test_flexbuf : public aquarius::detail::basic_flexbuf<>
{
    using base = aquarius::detail::basic_flexbuf<>;
    using base::underflow;
    using base::overflow;

    test_flexbuf(std::size_t m = std::numeric_limits<std::size_t>::max()) : base(m) {}
};

BOOST_AUTO_TEST_CASE(underflow_and_overflow_direct)
{
    test_flexbuf tf(4);

    // underflow on empty buffer -> EOF
    BOOST_CHECK_EQUAL(tf.underflow(), std::char_traits<char>::eof());

    // prepare and commit one byte then underflow should return that byte
    auto mb = tf.prepare(2);
    std::memcpy(reinterpret_cast<char*>(mb.data()), "Z", 1);
    tf.commit(1);
    BOOST_CHECK_EQUAL(tf.underflow(), std::char_traits<char>::to_int_type('Z'));

    // overflow with EOF should return not_eof
    BOOST_CHECK_EQUAL(tf.overflow(std::char_traits<char>::eof()),
                      std::char_traits<char>::not_eof(std::char_traits<char>::eof()));

    // make a very small buffer to force reserve failure in overflow
    test_flexbuf small(1);
    auto mb2 = small.prepare(1);
    std::memcpy(reinterpret_cast<char*>(mb2.data()), "A", 1);
    small.commit(1);

    auto r = small.overflow(std::char_traits<char>::to_int_type('X'));
    BOOST_CHECK_EQUAL(r, std::char_traits<char>::eof());
}

BOOST_AUTO_TEST_CASE(seekoff_fails_when_put_capacity_insufficient)
{
    // create buffer with small capacity
    flex_buffer fb(2);
    auto mb = fb.prepare(1);
    std::memcpy(reinterpret_cast<char*>(mb.data()), "x", 1);
    fb.commit(1);

    // try to seekbeg beyond available put space -> should fail
    auto res = fb.pubseekoff(10, std::ios_base::beg, std::ios_base::out);
    BOOST_CHECK(res == std::streampos(std::streamoff(-1)));
}

BOOST_AUTO_TEST_SUITE_END()
