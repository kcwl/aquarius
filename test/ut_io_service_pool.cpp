#include <boost/test/unit_test.hpp>
#include <aquarius/io_service_pool.hpp>

BOOST_AUTO_TEST_SUITE(ut_io_service_pool)

BOOST_AUTO_TEST_CASE(construction_and_access)
{
    aquarius::io_service_pool pool(1);
    BOOST_TEST(pool.size() == 1);

    auto& io = pool.get_io_service();
    (void)io;

    BOOST_TEST(pool.pool_size() == 1);
}

BOOST_AUTO_TEST_SUITE_END()
