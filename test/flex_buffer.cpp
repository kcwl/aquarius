#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/serialize/flex_buffer.hpp>

BOOST_AUTO_TEST_SUITE(serialize)

BOOST_AUTO_TEST_CASE(ctor)
{
	{
		aquarius::flex_buffer buffer{};

		BOOST_TEST(buffer.tellg() == 0);
		BOOST_TEST(buffer.tellp() == 0);
		BOOST_TEST(buffer.empty());
	}
	
	{
		char arr[] = "hello";

		aquarius::flex_buffer buffer((uint8_t*)&arr, 5);

		BOOST_TEST(buffer.tellg() == 5);

		std::string temp{};
		buffer.get(temp);

		BOOST_TEST(temp == "hello");
	}

	{
		char arr[] = "hello";
		aquarius::flex_buffer buffer((uint8_t*)&arr, 5);

		aquarius::flex_buffer buffer1(std::move(buffer));

		BOOST_TEST(buffer1.tellg() == 5);

		std::string temp{};
		buffer1.get(temp);

		BOOST_TEST(temp == "hello");

		BOOST_TEST(buffer.tellg() == 0);
		BOOST_TEST(buffer.tellp() == 0);
	}

	{
		char arr[] = "hello";
		aquarius::flex_buffer buffer((uint8_t*)&arr, 5);

		auto buffer1 = std::move(buffer);

		BOOST_TEST(buffer1.tellg() == 5);

		std::string temp{};
		buffer1.get(temp);

		BOOST_TEST(temp == "hello");

		BOOST_TEST(buffer.tellg() == 0);
		BOOST_TEST(buffer.tellp() == 0);
	}
}

BOOST_AUTO_TEST_CASE(member_function)
{
	{
		char arr[] = "hello";
		aquarius::flex_buffer buffer((uint8_t*)&arr, 5);

		auto begin = buffer.rdata();

		auto data = buffer.get();

		BOOST_TEST(*begin == data);
	}
}

BOOST_AUTO_TEST_SUITE_END()