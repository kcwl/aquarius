#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/member_pointer.hpp>
#include <string_view>

BOOST_AUTO_TEST_SUITE(member_pointer_tests)

BOOST_AUTO_TEST_CASE(member_variable_name)
{
    struct A { int val; void foo() {} };

    constexpr auto name = aquarius::get_member_pointer_name<A, &A::val>();
    std::string_view s = name;
    BOOST_TEST(!s.empty());
    BOOST_TEST(s.find("val") != std::string_view::npos);

    // also test the member_pointer_name helper
    constexpr auto name2 = aquarius::member_pointer_name<A, &A::val>::value;
    std::string_view s2 = name2;
    BOOST_TEST(!s2.empty());
    BOOST_TEST(s2.find("val") != std::string_view::npos);
}

BOOST_AUTO_TEST_CASE(member_function_name)
{
    struct B { int bar() { return 0; } };

    constexpr auto name = aquarius::get_member_pointer_name<B, &B::bar>();
    std::string_view s = name;
    BOOST_TEST(!s.empty());
    BOOST_TEST(s.find("bar") != std::string_view::npos);
}

BOOST_AUTO_TEST_CASE(static_member_name)
{
    struct C { int sv; };
    // take address of static member
    constexpr auto name = aquarius::get_member_pointer_name<C, &C::sv>();
    std::string_view s = name;
    BOOST_TEST(!s.empty());
    BOOST_TEST(s.find("sv") != std::string_view::npos);
}

BOOST_AUTO_TEST_SUITE_END()
