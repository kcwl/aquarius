#include <boost/test/unit_test.hpp>
#include <aquarius/serialize/json.hpp>

BOOST_AUTO_TEST_SUITE(ut_serialize_json)

using namespace std::string_view_literals;

BOOST_AUTO_TEST_CASE(json_array_roundtrip)
{
    std::vector<char> v{1, 2, 3, 4};

    auto val = aquarius::json_value_from_array(v);

    auto back = aquarius::json_value_to_array(val);

    BOOST_TEST(back == v);
}

BOOST_AUTO_TEST_CASE(json_value_from_object_and_basic_parse)
{
    std::string s = "hello";
    auto v = aquarius::json_value_from_object(s);
    BOOST_TEST(v.is_string());
}

BOOST_AUTO_TEST_CASE(check_to_array_failed)
{
    aquarius::json::value v{};
    auto& obj = v.emplace_object();
    obj.emplace("hello", "world");

    auto vec = aquarius::json_value_to_array(v);

    BOOST_TEST(vec.empty());
}

BOOST_AUTO_TEST_SUITE_END()
