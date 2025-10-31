#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/serialize/get_method.hpp>

BOOST_AUTO_TEST_SUITE(serialize)

BOOST_AUTO_TEST_CASE(get_method)
{
	aquarius::get_parse pr{};

	{
		aquarius::flex_buffer buffer{};

		pr.to_datas(123, buffer, "id");

		std::string tmp{};
		buffer.get(tmp);

		BOOST_TEST(tmp == "\"id\"=123");
	}

	{
		aquarius::flex_buffer buffer{};

		pr.to_datas(true, buffer, "sex");

		std::string tmp{};
		buffer.get(tmp);

		BOOST_TEST(tmp == "\"sex\"=true");
	}

	{
		aquarius::flex_buffer buffer{};

		pr.to_datas<std::string>("hello", buffer, "name");

		std::string tmp{};
		buffer.get(tmp);

		BOOST_TEST(tmp == "\"name\"=\"hello\"");
	}

	{
		std::string result = "\"name\"=\"hello\"";
		aquarius::flex_buffer buffer((uint8_t*)result.data(), result.size());
		auto res = pr.from_datas<std::string>(buffer, "name");

		BOOST_TEST(res == "hello");
	}
	{
		std::string result = "\"sex\"=true";
		aquarius::flex_buffer buffer((uint8_t*)result.data(), result.size());
		auto res = pr.from_datas<bool>(buffer, "sex");

		BOOST_TEST(res);
	}
	{
		std::string result = "\"id\"=123";
		aquarius::flex_buffer buffer((uint8_t*)result.data(), result.size());
		auto res = pr.from_datas<int>(buffer,"id");

		BOOST_TEST(res == 123);
	}
}

BOOST_AUTO_TEST_SUITE_END()