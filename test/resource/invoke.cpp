#define BOOST_TEST_NO_MAIN
#include <aquarius/resource/config_tag_invoke.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

struct mock_config
{
	int a;
	std::string b;
};


CONFIG_MICRO(mock_config, cfg)
{
	cfg.a = 1;
	cfg.b = "test";
}

BOOST_AUTO_TEST_SUITE(ut_resource)

BOOST_AUTO_TEST_CASE(invoke)
{
	mock_config cfg{};

	cfg_value_from<mock_config>(cfg);

	BOOST_TEST(cfg.a == 1);
	BOOST_TEST(cfg.b == "test");
}

BOOST_AUTO_TEST_SUITE_END()