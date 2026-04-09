#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>

#include "aquarius/module/player_module.hpp"
#include "aquarius/module/module_router.hpp"

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(module_router_suite)

BOOST_AUTO_TEST_CASE(test_player_module_register_and_schedule)
{
    // Ensure player_module is registered
    module_router::get_mutable_instance().regist<player_module>();

    // Insert a player with id 1 via schedule returning int
    module_router::get_mutable_instance().schedule<player_module, int>([](player_module* m) {
        m->insert(1, std::make_shared<player_base>());
        return 0;
    });

    // Retrieve the player and check not null
    auto p = module_router::get_mutable_instance().schedule<player_module, std::shared_ptr<player_base>>(
        [](player_module* m) { return m->get<player_base>(1); });

    BOOST_TEST((bool)p);

    // Erase the player returning int
    module_router::get_mutable_instance().schedule<player_module, int>([](player_module* m) { m->erase(1); return 0; });

    // Now get should return nullptr
    auto p2 = module_router::get_mutable_instance().schedule<player_module, std::shared_ptr<player_base>>(
        [](player_module* m) { return m->get<player_base>(1); });

    BOOST_TEST(!p2);
}

BOOST_AUTO_TEST_CASE(test_schedule_module_not_found)
{
    struct not_registered_module {};

    // No registration for not_registered_module, scheduling should return default-initialized int (0)
    int v = module_router::get_mutable_instance().schedule<not_registered_module, int>([](not_registered_module*) { return 123; });

    BOOST_TEST(v == 0);
}

BOOST_AUTO_TEST_SUITE_END()
