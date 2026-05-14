#include <boost/test/unit_test.hpp>
#include <aquarius/module/player_module.hpp>

BOOST_AUTO_TEST_SUITE(ut_module_player_module)

BOOST_AUTO_TEST_CASE(insert_get_erase)
{
    aquarius::player_module pm;

    struct MyPlayer : public aquarius::player_base { int v = 42; };

    auto p = std::make_shared<MyPlayer>();
    pm.insert(7, p);

    auto got = pm.get<MyPlayer>(7);
    BOOST_TEST(got != nullptr);
    BOOST_TEST(got->v == 42);

    pm.erase(7);
    auto gone = pm.get<MyPlayer>(7);
    BOOST_TEST(gone == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()
