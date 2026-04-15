#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/concat.hpp>
#include <string_view>
#include <array>

BOOST_AUTO_TEST_SUITE(concat)

using namespace std::string_view_literals;

// Define NTTP-able string_views at namespace scope (external linkage)
inline constexpr std::string_view value1 = "hello "sv;
inline constexpr std::string_view value2 = "world"sv;
inline constexpr std::string_view single = "single"sv;
inline constexpr std::string_view a = "A"sv;
inline constexpr std::string_view empty = ""sv;
inline constexpr std::string_view b = "BC"sv;
inline constexpr std::string_view with_nul = "a\0b"sv; // 'a','\0','b'
inline constexpr std::string_view c = "c"sv;

BOOST_AUTO_TEST_CASE(concats)
{
	// two-part concat (runtime checks)
	auto result = aquarius::concat_v<value1, value2>;
	BOOST_TEST(result == "hello world"sv);

	// single-part concat should return the original
	auto single_result = aquarius::concat_v<single>;
	BOOST_TEST(single_result == "single"sv);

	// zero-part concat should produce an empty string_view
	auto empty_result = aquarius::concat_v<>;
	BOOST_TEST(empty_result.empty());

	// multi-part concat including an empty middle part
	auto multi = aquarius::concat_v<a, empty, b>;
	BOOST_TEST(multi == "ABC"sv);

	// concat with embedded NUL character inside one piece (runtime checks only)
    auto nul_concat = aquarius::concat_v<with_nul, c>;
	BOOST_TEST(nul_concat.size() == with_nul.size() + 1);
	BOOST_TEST(nul_concat == "a\0bc"sv);
}

BOOST_AUTO_TEST_CASE(impl_and_arr_inspection)
{
	// Explicitly call impl() for a two-part concat and inspect the returned array
	auto arr = aquarius::detail::concat<value1, value2>::impl();
	// The returned array contains the characters plus a trailing NUL
	constexpr auto expected = "hello world"sv;
	// check size: expected chars + terminating NUL
	BOOST_TEST(arr.size() == expected.size() + 1);
	// verify content up to terminating NUL
	for (std::size_t i = 0; i < expected.size(); ++i)
		BOOST_TEST(arr[i] == expected[i]);
	BOOST_TEST(arr[expected.size()] == '\0');

	// Access the static arr member and the value string_view for single and empty packs
	auto single_arr = aquarius::detail::concat<single>::arr;
	BOOST_TEST(single_arr.size() == single.size() + 1);
	BOOST_TEST(aquarius::concat_v<single> == "single"sv);

	auto empty_arr = aquarius::detail::concat<>::impl();
	// empty pack -> array with only terminating NUL
	BOOST_TEST(empty_arr.size() == 1u);
	BOOST_TEST(empty_arr[0] == '\0');
	BOOST_TEST((aquarius::concat_v<>).empty());
}

BOOST_AUTO_TEST_SUITE_END()