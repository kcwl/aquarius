#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/error.hpp>

BOOST_AUTO_TEST_SUITE(ut_error)

BOOST_AUTO_TEST_CASE(error_string_and_parse)
{
    BOOST_TEST(aquarius::ip::error_to_string(aquarius::ip::error::commit) == "commit error when parse proto");
    BOOST_TEST(aquarius::ip::error_to_string(aquarius::ip::error::consume) == "consume error when parse proto");
    BOOST_TEST(aquarius::ip::error_to_string(aquarius::ip::error::success) == "success");
}

BOOST_AUTO_TEST_CASE(status_category)
{
    auto& category = aquarius::ip::get_proto_error_category();

    BOOST_TEST(category.name() == "proto_error_category");

    BOOST_TEST(category.message(1) == "commit error when parse proto");
}

BOOST_AUTO_TEST_SUITE_END()
