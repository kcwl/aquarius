#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <string_view>
#include <type_traits>

BOOST_AUTO_TEST_SUITE(string_literals)

using namespace std::string_view_literals;

BOOST_AUTO_TEST_CASE(literals)
{
	constexpr auto str = aquarius::detail::bind_param<"hello">::value;

	static_assert(str == "hello"sv);

	BOOST_TEST(str == "hello"sv);
}

BOOST_AUTO_TEST_CASE(empty_literal)
{
	constexpr auto s = aquarius::detail::bind_param<"">::value;

	static_assert(s == ""sv);
	BOOST_TEST(s.empty());
}

BOOST_AUTO_TEST_CASE(constructor_and_members)
{
	// "hello" has length 6 including null terminator
	constexpr aquarius::detail::string_literal<6> lit("hello");

	static_assert(lit.size == 6);
	static_assert(lit.value[0] == 'h');
	static_assert(lit.value[4] == 'o');
	static_assert(lit.value[5] == '\0');

	BOOST_TEST(lit.size == 6);
	BOOST_TEST(lit.value[0] == 'h');
	BOOST_TEST(lit.value[4] == 'o');
	BOOST_TEST(lit.value[5] == '\0');
}

BOOST_AUTO_TEST_CASE(embedded_null)
{
	// Create an array with an embedded null and ensure the buffer stores it
	constexpr char arr[] = {'a', '\0', 'b', '\0'}; // size 4
	constexpr aquarius::detail::string_literal<4> lit(arr);

    // std::string_view constructed from C-string stops at first null
	auto sv = std::string_view(lit.value);

	static_assert(lit.size == 4);

	BOOST_TEST(lit.size == 4);
	BOOST_TEST(sv == "a"sv);
	// Ensure underlying buffer keeps the character after the embedded null
	BOOST_TEST(lit.value[2] == 'b');
}

BOOST_AUTO_TEST_CASE(multiple_bind_params)
{
	constexpr auto s1 = aquarius::detail::bind_param<"A B">::value;
	constexpr auto s2 = aquarius::detail::bind_param<"12345">::value;

	static_assert(s1 == "A B"sv);
	static_assert(s2 == "12345"sv);

	BOOST_TEST(s1 == "A B"sv);
	BOOST_TEST(s2 == "12345"sv);
}

BOOST_AUTO_TEST_CASE(size_static_and_bind_length)
{
	// Ensure the static size member is available at compile time and via the type
	static_assert(aquarius::detail::string_literal<6>::size == 6);
	BOOST_TEST(aquarius::detail::string_literal<6>::size == 6);

	// bind_param should construct the string_view with explicit length (exclude null)
	constexpr auto sv = aquarius::detail::bind_param<"hello">::value;
	static_assert(sv.size() == 5);
	BOOST_TEST(sv.size() == 5);

	// Single character literal
	constexpr auto s_single = aquarius::detail::bind_param<"x">::value;
	static_assert(s_single == "x"sv);
	BOOST_TEST(s_single == "x"sv);
}

BOOST_AUTO_TEST_SUITE_END()
