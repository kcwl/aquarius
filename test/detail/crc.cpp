#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/crc.hpp>

BOOST_AUTO_TEST_SUITE(crc)

BOOST_AUTO_TEST_CASE(crc_basic)
{
    const char data[] = "hello";
    auto checksum = aquarius::detail::crc32(data, 5);
    // Known CRC32 for "hello" is 0x3610a686 (little-endian value may vary),
    // but we'll check consistency against boost's crc computed directly here.
    boost::crc_32_type crc;
    crc.process_bytes(data, 5);
    BOOST_TEST(checksum == crc.checksum());
}

BOOST_AUTO_TEST_CASE(crc_empty)
{
    const char data[] = "";
    auto checksum = aquarius::detail::crc32(data, 0);
    boost::crc_32_type crc;
    crc.process_bytes(data, 0);
    BOOST_TEST(checksum == crc.checksum());
}

BOOST_AUTO_TEST_SUITE_END()
