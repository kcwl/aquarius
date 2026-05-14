#include <boost/test/unit_test.hpp>
#include <aquarius/error_code.hpp>
#include <type_traits>
#include <boost/system/error_code.hpp>

BOOST_AUTO_TEST_SUITE(ut_error_code)

BOOST_AUTO_TEST_CASE(type_alias)
{
    BOOST_TEST((std::is_same_v<aquarius::error_code, boost::system::error_code>));
}

BOOST_AUTO_TEST_SUITE_END()
