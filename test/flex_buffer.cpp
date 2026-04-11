#define BOOST_NO_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "../include/aquarius/detail/flex_buffer.hpp"
#include <boost/asio/buffer.hpp>
#include <iostream>

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

BOOST_AUTO_TEST_SUITE_END()
