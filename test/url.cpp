#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/ip/http/url_encode.hpp>

BOOST_AUTO_TEST_SUITE(url)

BOOST_AUTO_TEST_CASE(encode)
{
	{
		std::u8string str = u8"hello word!";

		auto encode_seq = aquarius::url_u8_encode(str);

		auto decode_seq = aquarius::url_u8_decode(encode_seq);

		BOOST_CHECK(str == decode_seq);
	}
	
	{
		std::u8string str = u8"ÄãºÃ";
		std::string astr = "ÄãºÃ";


		auto encode_seq = aquarius::url_u8_encode(str);

		auto decode_seq = aquarius::url_u8_decode(encode_seq);

		BOOST_CHECK(str == decode_seq);
	}
}

BOOST_AUTO_TEST_SUITE_END()
