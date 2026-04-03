#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/tbl/empty.hpp>
#include <aquarius/asio.hpp>

BOOST_AUTO_TEST_SUITE(empty_impl_tests)

BOOST_AUTO_TEST_CASE(test_empty_methods)
{
	aquarius::tbl::database_param param;
	aquarius::tbl::empty_impl impl(param);

	impl.enable_transaction();
	impl.disable_transaction();
	
	BOOST_TEST(impl.begin().value() == 0);
	BOOST_TEST(impl.commit().value() == 0);
	BOOST_TEST(impl.rollback().value() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
