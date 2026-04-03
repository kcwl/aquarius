#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/uuid_generator.hpp>

BOOST_AUTO_TEST_SUITE(uuid_generator_tests)

BOOST_AUTO_TEST_CASE(generate_uuid)
{
	aquarius::detail::uuid_generator gen;
	uint32_t val1 = gen();
	uint32_t val2 = gen();
	
	// 在绝大几率下两次生成的 UUID 应该不同
	BOOST_TEST(val1 != val2);
}

BOOST_AUTO_TEST_SUITE_END()
