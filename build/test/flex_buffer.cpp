#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <string>

BOOST_AUTO_TEST_SUITE(flex_buffer_tests)

BOOST_AUTO_TEST_CASE(buffer_operations)
{
	aquarius::flex_buffer buf;
	BOOST_TEST(buf.size() == 0);
	
	auto mb = buf.prepare(10);
	BOOST_TEST(buf.size() == 0);
	
	buf.commit(5);
	BOOST_TEST(buf.size() == 5);
	
	buf.consume(2);
	BOOST_TEST(buf.size() == 3);
	
	buf.reset(100);
	BOOST_TEST(buf.size() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
