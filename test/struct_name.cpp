#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/struct_name.hpp>
#include <string_view>

BOOST_AUTO_TEST_SUITE(struct_names)

using namespace std::string_view_literals;

struct temp_struct
{

};

BOOST_AUTO_TEST_CASE(name)
{
	constexpr auto str = aquarius::detail::struct_name<temp_struct>();

	static_assert(str == "temp_struct"sv);

	BOOST_TEST(str == "temp_struct"sv);
}

BOOST_AUTO_TEST_SUITE_END()