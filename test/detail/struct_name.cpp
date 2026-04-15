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

// Test type printed with a leading "class" keyword at global scope
class global_class_test {};

BOOST_AUTO_TEST_CASE(class_global_name)
{
	constexpr auto str = aquarius::detail::struct_name<global_class_test>();

	static_assert(str == "global_class_test"sv);

	BOOST_TEST(str == "global_class_test"sv);
}

// Test namespaced type where the parser must strip namespace qualifiers
namespace ns_for_test
{
	class namespaced_class {};
}

BOOST_AUTO_TEST_CASE(namespaced_name)
{
	constexpr auto str = aquarius::detail::struct_name<ns_for_test::namespaced_class>();

	static_assert(str == "namespaced_class"sv);

	BOOST_TEST(str == "namespaced_class"sv);
}

BOOST_AUTO_TEST_CASE(builtin_type_name)
{
	constexpr auto str = aquarius::detail::struct_name<int>();

	static_assert(str == "int"sv);

	BOOST_TEST(str == "int"sv);
}

BOOST_AUTO_TEST_SUITE_END()