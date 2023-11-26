#pragma once
#include "generate.h"

#include <aquarius.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(elastic)

struct person
{
	int a;
	char b;
};

BOOST_AUTO_TEST_CASE(tuple_size)
{
	static_assert(aquarius::tuple_size_v<person> == 2, "size error!");

	static_assert(aquarius::tuple_size_v<std::array<int, 4>> == 4, "size error!");

	static_assert(aquarius::tuple_size_v<std::tuple<int, char>> == 2, "size error!");
}

BOOST_AUTO_TEST_CASE(reflect)
{
	constexpr person t3{ 1, 'a' };

	static_assert(aquarius::get<0>(t3) == 1, "get value error!");
	static_assert(aquarius::get<1>(t3) == 'a', "get value error!");
	static_assert(aquarius::name<person>() == "person"sv, "get name error!");
}

BOOST_AUTO_TEST_CASE(basic_type)
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

struct base
{
	int a;
	int b;
};

struct deri : base
{
	int c;
	int d;

private:
	friend class aquarius::access;

	template <typename _Archive>
	void serialize(_Archive& ar)
	{
		ar& aquarius::base_object<base>(*this);
		ar& c;
		ar& d;
	}
};

BOOST_AUTO_TEST_CASE(inherit)
{
	{
		deri d{};
		d.a = 1;
		d.b = 2;
		d.c = 3;
		d.d = 4;

		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		oa << d;

		deri dd{};
		aquarius::binary_iarchive ia(buf);
		ia >> dd;

		BOOST_CHECK(d.a == dd.a && d.b == dd.b && d.c == dd.c && d.d == dd.d);
	}

	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		struct son
		{
			int b_;
		};

		struct person
		{
			int a_;
			int b_;
			son s_;
		};

		person p_in{ 1, 2, { 3 } };

		oa << p_in;

		aquarius::binary_iarchive ia(buf);
		person p_out{};
		ia >> p_out;

		BOOST_TEST(p_in.a_ == p_out.a_);
		BOOST_TEST(p_in.b_ == p_out.b_);
		BOOST_TEST(p_in.s_.b_ == p_out.s_.b_);
	}

	{
		struct son
		{
			int b_;
		};

		struct person
		{
			int a_;
			int b_;
			son s_;
		};

		aquarius::flex_buffer_t buf;

		aquarius::binary_oarchive oa(buf);

		person p{};
		p.a_ = 1;
		p.b_ = 2;

		oa << p.a_;
		oa << p.b_;

		aquarius::binary_iarchive ia(buf);

		person p1{};

		ia >> p1;

		BOOST_CHECK_EQUAL(p1.a_, 0);
		BOOST_CHECK_EQUAL(p1.b_, 0);
		BOOST_CHECK_EQUAL(p1.s_.b_, 0);
	}
}

BOOST_AUTO_TEST_CASE(property)
{
	{
		std::optional<int> value{ 1 };

		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		oa << value;

		std::optional<int> value1;

		aquarius::binary_iarchive ia(buf);
		ia >> value1;

		BOOST_TEST(value.value() == value1.value());
	}
	{
		aquarius::fixed<uint32_t> value{};
		value = 2;

		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		oa << value;

		aquarius::fixed<uint32_t> value1{};

		aquarius::binary_iarchive ia(buf);
		ia >> value1;

		BOOST_TEST(*value == *value1);
	}

	{
		aquarius::fixed<uint64_t> value{};
		value = 2;

		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		oa << value;

		aquarius::fixed<uint64_t> value1{};

		aquarius::binary_iarchive ia(buf);
		ia >> value1;

		BOOST_CHECK_EQUAL(*value, *value1);
	}
	{
		aquarius::flex_buffer_t buf;
		aquarius::binary_oarchive oa(buf);

		std::vector<std::byte> a_in = { std::byte(1), std::byte(2), std::byte(3), std::byte(4), std::byte(5) };

		oa << a_in;

		std::vector<std::byte> a_out{};

		aquarius::binary_iarchive ia(buf);
		ia >> a_out;

		BOOST_TEST(a_in == a_out);
	}
}

BOOST_AUTO_TEST_SUITE_END()