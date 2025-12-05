#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/crc.hpp>
#include <aquarius/detail/io_service_pool.hpp>
#include <aquarius/detail/make_endpoint.hpp>
#include <aquarius/detail/session_store.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <string_view>
#include <csignal>

using namespace std::string_view_literals;

BOOST_AUTO_TEST_SUITE(details)


//BOOST_AUTO_TEST_CASE(flex_buffer)
//{
//	aquarius::flex_buffer buffer{};
//
//	auto begin = buffer.get();
//
//	BOOST_CHECK_EQUAL(begin, '\0');
//
//	buffer.consume(8191);
//
//	BOOST_CHECK_THROW(buffer.get(), aquarius::error_code);
//
//	buffer.commit(8192);
//
//	BOOST_CHECK_THROW(buffer.put(1), aquarius::error_code);
//
//	BOOST_CHECK(buffer.empty());
//}




BOOST_AUTO_TEST_SUITE_END()