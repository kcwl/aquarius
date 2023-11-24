#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius/elastic.hpp>

BOOST_AUTO_TEST_SUITE(io)

BOOST_AUTO_TEST_CASE(elastic_type)
{
	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		char a_in = (std::numeric_limits<char>::max)();

		oa << a_in;

		char a_out{};

		aquarius::binary_iarchive ia(buf);
		ia >> a_out;

		BOOST_TEST(a_in == a_out);
	}

	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		int8_t a_in = (std::numeric_limits<int8_t>::max)();

		oa << a_in;

		int8_t a_out{};

		aquarius::binary_iarchive ia(buf);
		ia >> a_out;

		BOOST_TEST(a_in == a_out);
	}

	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		uint8_t a_in = (std::numeric_limits<uint8_t>::max)();

		oa << a_in;

		uint8_t a_out{};

		aquarius::binary_iarchive ia(buf);
		ia >> a_out;

		BOOST_TEST(a_in == a_out);
	}

	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		int16_t a_in = (std::numeric_limits<int16_t>::max)();

		oa << a_in;

		int16_t a_out{};

		aquarius::binary_iarchive ia(buf);
		ia >> a_out;

		BOOST_TEST(a_in == a_out);
	}
	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		uint16_t a_in = (std::numeric_limits<uint16_t>::max)();

		oa << a_in;

		uint16_t a_out{};

		aquarius::binary_iarchive ia(buf);
		ia >> a_out;

		BOOST_TEST(a_in == a_out);
	}

	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		int32_t a_in = (std::numeric_limits<int32_t>::max)();

		oa << a_in;

		int32_t a_out{};

		aquarius::binary_iarchive ia(buf);
		ia >> a_out;

		BOOST_TEST(a_in == a_out);
	}

	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		uint32_t a_in = (std::numeric_limits<uint32_t>::max)();

		oa << a_in;

		uint32_t a_out{};

		aquarius::binary_iarchive ia(buf);
		ia >> a_out;

		BOOST_TEST(a_in == a_out);
	}

	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		int64_t a_in = (std::numeric_limits<int64_t>::max)();

		oa << a_in;

		int64_t a_out{};

		aquarius::binary_iarchive ia(buf);
		ia >> a_out;

		BOOST_TEST(a_in == a_out);
	}

	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		uint64_t a_in = (std::numeric_limits<uint64_t>::max)();

		oa << a_in;

		uint64_t a_out{};

		aquarius::binary_iarchive ia(buf);
		ia >> a_out;

		BOOST_TEST(a_in == a_out);
	}
	{
		enum class color
		{
			red = 1,
			blue = 2
		};

		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		oa << color::red;

		color cr{};

		aquarius::binary_iarchive ia(buf);
		ia >> cr;

		BOOST_CHECK(cr == color::red);
	}
	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		std::string a_in = "hello world!";

		oa << a_in;

		std::string a_out{};

		aquarius::binary_iarchive ia(buf);
		ia >> a_out;

		BOOST_TEST(a_in == a_out);
	}
	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		bool a_in = false;

		oa << a_in;

		bool a_out = true;

		aquarius::binary_iarchive ia(buf);

		ia >> a_out;

		BOOST_TEST(a_in == a_out);
	}
	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		std::vector<std::byte> a_in = { std::byte('1'), std::byte('2'), std::byte('3'), std::byte('4'),
										std::byte('5') };

		oa << a_in;

		std::vector<std::byte> a_out{};

		aquarius::binary_iarchive ia(buf);
		ia >> a_out;

		BOOST_TEST(a_in == a_out);
	}
	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		double a_in = 1.2;

		oa << a_in;

		double a_out;

		aquarius::binary_iarchive ia(buf);

		ia >> a_out;

		BOOST_TEST(a_in == a_out);
	}
	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		float a_in = 2.4f;

		oa << a_in;

		float a_out;

		aquarius::binary_iarchive ia(buf);

		ia >> a_out;

		BOOST_TEST(a_in == a_out);
	}
}

BOOST_AUTO_TEST_SUITE_END()