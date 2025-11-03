#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/crc.hpp>

BOOST_AUTO_TEST_SUITE(crc)

BOOST_AUTO_TEST_CASE(crc_check)
{
	std::string str("hello world");

	auto result = aquarius::detail::crc32(str.c_str(), str.size());

	BOOST_TEST(result == (uint32_t)0x0d4a1185);
}

BOOST_AUTO_TEST_SUITE_END()