#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/pfr.hpp>
#include <vector>
#include <string>
#include <type_traits>
#include <aquarius.hpp>

using namespace std::string_view_literals;

// Test aggregate for SQL view testing
struct person
{
    int id;
    int age;
};

// ===================================================================
// concepts_tests
// ===================================================================
BOOST_AUTO_TEST_SUITE(tbl_concepts_tests)

BOOST_AUTO_TEST_CASE(remove_member_pointer_plain_type)
{
    BOOST_TEST((std::is_same_v<aquarius::remove_member_pointer<int>::type, int>));
}

BOOST_AUTO_TEST_CASE(remove_member_pointer_double_type)
{
    BOOST_TEST((std::is_same_v<aquarius::remove_member_pointer<double>::type, double>));
}

BOOST_AUTO_TEST_CASE(remove_member_pointer_member_ptr)
{
    BOOST_TEST((std::is_same_v<aquarius::remove_member_pointer<int(person::*)>::type, int>));
}

BOOST_AUTO_TEST_CASE(member_pointer_struct_plain_type)
{
    BOOST_TEST((std::is_same_v<aquarius::member_pointer_struct<int>::type, int>));
}

BOOST_AUTO_TEST_CASE(member_pointer_struct_member_ptr)
{
    BOOST_TEST((std::is_same_v<aquarius::member_pointer_struct<int(person::*)>::type, person>));
}

BOOST_AUTO_TEST_CASE(iterator_t_with_vector)
{
    BOOST_TEST(aquarius::iterator_t<std::vector<int>>);
}

BOOST_AUTO_TEST_CASE(iterator_t_with_string)
{
    BOOST_TEST(aquarius::iterator_t<std::string>);
}

BOOST_AUTO_TEST_CASE(iterator_t_with_non_iterable)
{
    BOOST_TEST(!aquarius::iterator_t<int>);
}

BOOST_AUTO_TEST_CASE(push_t_with_vector)
{
    BOOST_TEST(aquarius::push_t<std::vector<int>>);
}

BOOST_AUTO_TEST_CASE(push_t_with_non_pushable)
{
    BOOST_TEST(!aquarius::push_t<int>);
}

BOOST_AUTO_TEST_SUITE_END()

// ===================================================================
// struct_name_tests
// ===================================================================
BOOST_AUTO_TEST_SUITE(tbl_struct_name_tests)

BOOST_AUTO_TEST_CASE(person_struct_name)
{
    constexpr auto name = aquarius::detail::struct_name<person>();
    BOOST_TEST(name == "person");
}

BOOST_AUTO_TEST_SUITE_END()

// ===================================================================
// concat_tests
// ===================================================================
BOOST_AUTO_TEST_SUITE(tbl_concat_tests)

BOOST_AUTO_TEST_CASE(concat_two_strings)
{
    constexpr static auto a = "hello"sv;
    constexpr static auto b = "world"sv;
    constexpr auto result = aquarius::concat_v<a, b>;
    BOOST_TEST(result == "helloworld");
}

BOOST_AUTO_TEST_CASE(concat_three_strings)
{
    constexpr static auto x = "a"sv;
    constexpr static auto y = "b"sv;
    constexpr static auto z = "c"sv;
    constexpr auto result = aquarius::concat_v<x, y, z>;
    BOOST_TEST(result == "abc");
}

BOOST_AUTO_TEST_CASE(concat_with_empty)
{
    constexpr static auto empty = ""sv;
    constexpr static auto text = "hello"sv;
    constexpr auto result = aquarius::concat_v<empty, text>;
    BOOST_TEST(result == "hello");
}

BOOST_AUTO_TEST_CASE(concat_single_string)
{
    constexpr static auto single = "only"sv;
    constexpr auto result = aquarius::concat_v<single>;
    BOOST_TEST(result == "only");
}

BOOST_AUTO_TEST_SUITE_END()

// ===================================================================
// member_pointer_tests
// ===================================================================
BOOST_AUTO_TEST_SUITE(tbl_member_pointer_tests)

BOOST_AUTO_TEST_CASE(get_member_name_id)
{
    constexpr auto name = aquarius::get_member_pointer_name<person, &person::id>();
    BOOST_TEST(name == "id");
}

BOOST_AUTO_TEST_CASE(get_member_name_age)
{
    constexpr auto name = aquarius::get_member_pointer_name<person, &person::age>();
    BOOST_TEST(name == "age");
}

BOOST_AUTO_TEST_CASE(member_pointer_name_value_id)
{
    constexpr auto name = aquarius::member_pointer_name<person, &person::id>::value;
    BOOST_TEST(name == "id");
}

BOOST_AUTO_TEST_CASE(member_pointer_name_value_age)
{
    constexpr auto name = aquarius::member_pointer_name<person, &person::age>::value;
    BOOST_TEST(name == "age");
}

BOOST_AUTO_TEST_SUITE_END()

// ===================================================================
// grep_view_tests
// ===================================================================
BOOST_AUTO_TEST_SUITE(tbl_grep_view_tests)

BOOST_AUTO_TEST_CASE(grep_equal_method)
{
    aquarius::grep_view<&person::id> g;
    g.equal(42);
    std::string result = static_cast<std::string>(g);
    BOOST_TEST(result == "id = 42");
}

BOOST_AUTO_TEST_CASE(grep_equal_operator)
{
    aquarius::grep_view<&person::id> g;
    g.operator==(42);
    std::string result = static_cast<std::string>(g);
    BOOST_TEST(result == "id = 42");
}

BOOST_AUTO_TEST_CASE(grep_less_method)
{
    aquarius::grep_view<&person::age> g;
    g.less(30);
    std::string result = static_cast<std::string>(g);
    BOOST_TEST(result == "age < 30");
}

BOOST_AUTO_TEST_CASE(grep_less_operator)
{
    aquarius::grep_view<&person::age> g;
    g.operator<(30);
    std::string result = static_cast<std::string>(g);
    BOOST_TEST(result == "age < 30");
}

BOOST_AUTO_TEST_CASE(grep_less_equal_method)
{
    aquarius::grep_view<&person::age> g;
    g.less_equal(30);
    std::string result = static_cast<std::string>(g);
    BOOST_TEST(result == "age <= 30");
}

BOOST_AUTO_TEST_CASE(grep_less_equal_operator)
{
    aquarius::grep_view<&person::age> g;
    g.operator<=(30);
    std::string result = static_cast<std::string>(g);
    BOOST_TEST(result == "age <= 30");
}

BOOST_AUTO_TEST_CASE(grep_greater_method)
{
    aquarius::grep_view<&person::id> g;
    g.greater(10);
    std::string result = static_cast<std::string>(g);
    BOOST_TEST(result == "id > 10");
}

BOOST_AUTO_TEST_CASE(grep_greater_equal_method)
{
    aquarius::grep_view<&person::id> g;
    g.greater_equal(10);
    std::string result = static_cast<std::string>(g);
    BOOST_TEST(result == "id >= 10");
}

BOOST_AUTO_TEST_CASE(grep_greater_equal_operator)
{
    aquarius::grep_view<&person::id> g;
    g.operator>=(10);
    std::string result = static_cast<std::string>(g);
    BOOST_TEST(result == "id >= 10");
}

BOOST_AUTO_TEST_CASE(grep_no_aggr_no_parens)
{
    aquarius::grep_view<&person::id> g;
    g.equal(5);
    std::string result = static_cast<std::string>(g);
    BOOST_TEST(result.front() != '(');
    BOOST_TEST(result.back() != ')');
}

BOOST_AUTO_TEST_CASE(grep_or_wraps_in_parens)
{
    aquarius::grep_view<&person::id> g1;
    g1.equal(1);
    aquarius::grep_view<&person::id> g2;
    g2.equal(2);
    g1 || g2;
    std::string result = static_cast<std::string>(g1);
    BOOST_TEST(result.find("or") != std::string::npos);
    BOOST_TEST(result.front() == '(');
    BOOST_TEST(result.back() == ')');
}

BOOST_AUTO_TEST_CASE(grep_or_contains_both_conditions)
{
    aquarius::grep_view<&person::id> g1;
    g1.equal(1);
    aquarius::grep_view<&person::age> g2;
    g2.equal(25);
    g1 || g2;
    std::string result = static_cast<std::string>(g1);
    BOOST_TEST(result.find("id = 1") != std::string::npos);
    BOOST_TEST(result.find("age = 25") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(grep_overwrite_condition)
{
    aquarius::grep_view<&person::id> g;
    g.equal(1);
    g.equal(2);
    std::string result = static_cast<std::string>(g);
    BOOST_TEST(result == "id = 2");
}

BOOST_AUTO_TEST_CASE(grep_call_operator_returns_fresh_instance)
{
    auto g = aquarius::grep<&person::id>();
    g.equal(42);
    auto g2 = g();
    g2.equal(10);
    std::string r1 = static_cast<std::string>(g);
    std::string r2 = static_cast<std::string>(g2);
    BOOST_TEST(r1 == "id = 42");
    BOOST_TEST(r2 == "id = 10");
}

BOOST_AUTO_TEST_CASE(grep_chained_less_returns_self)
{
    aquarius::grep_view<&person::age> g;
    auto& ref = g.less(30);
    BOOST_TEST(&ref == &g);
}

BOOST_AUTO_TEST_CASE(grep_chained_less_equal_returns_self)
{
    aquarius::grep_view<&person::age> g;
    auto& ref = g.less_equal(30);
    BOOST_TEST(&ref == &g);
}

BOOST_AUTO_TEST_CASE(grep_chained_greater_returns_self)
{
    aquarius::grep_view<&person::age> g;
    auto& ref = g.greater(30);
    BOOST_TEST(&ref == &g);
}

BOOST_AUTO_TEST_CASE(grep_chained_greater_equal_returns_self)
{
    aquarius::grep_view<&person::age> g;
    auto& ref = g.greater_equal(30);
    BOOST_TEST(&ref == &g);
}

BOOST_AUTO_TEST_CASE(grep_chained_equal_returns_self)
{
    aquarius::grep_view<&person::age> g;
    auto& ref = g.equal(30);
    BOOST_TEST(&ref == &g);
}

BOOST_AUTO_TEST_SUITE_END()

// ===================================================================
// insert_view_tests
// ===================================================================
BOOST_AUTO_TEST_SUITE(tbl_insert_view_tests)

BOOST_AUTO_TEST_CASE(insert_generates_sql)
{
    aquarius::insert_view iv;
    iv(person{1, 25});
    std::string sql = static_cast<std::string>(iv);
    BOOST_TEST(sql.find("insert into") != std::string::npos);
    BOOST_TEST(sql.find("person") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(insert_contains_field_names)
{
    aquarius::insert_view iv;
    iv(person{1, 25});
    std::string sql = static_cast<std::string>(iv);
    BOOST_TEST(sql.find("id") != std::string::npos);
    BOOST_TEST(sql.find("age") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(insert_contains_values)
{
    aquarius::insert_view iv;
    iv(person{42, 99});
    std::string sql = static_cast<std::string>(iv);
    BOOST_TEST(sql.find("values(") != std::string::npos);
    BOOST_TEST(sql.find("42") != std::string::npos);
    BOOST_TEST(sql.find("99") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(insert_overwrite_on_second_call)
{
    aquarius::insert_view iv;
    iv(person{1, 25});
    iv(person{2, 30});
    std::string sql = static_cast<std::string>(iv);
    BOOST_TEST(sql.find("2") != std::string::npos);
    BOOST_TEST(sql.find("30") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(insert_returns_self)
{
    aquarius::insert_view iv;
    auto& ref = iv(person{1, 25});
    BOOST_TEST(&ref == &iv);
}

BOOST_AUTO_TEST_CASE(insert_ends_with_closing_paren)
{
    aquarius::insert_view iv;
    iv(person{1, 25});
    std::string sql = static_cast<std::string>(iv);
    BOOST_TEST(sql.back() == ')');
}

BOOST_AUTO_TEST_SUITE_END()

// ===================================================================
// update_view_tests
// ===================================================================
BOOST_AUTO_TEST_SUITE(tbl_update_view_tests)

BOOST_AUTO_TEST_CASE(update_generates_sql)
{
    aquarius::update_view uv;
    uv(person{1, 25});
    std::string sql = static_cast<std::string>(uv);
    BOOST_TEST(sql.find("update person") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(update_contains_set_keyword)
{
    aquarius::update_view uv;
    uv(person{1, 25});
    std::string sql = static_cast<std::string>(uv);
    BOOST_TEST(sql.find("set") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(update_contains_field_names)
{
    aquarius::update_view uv;
    uv(person{1, 25});
    std::string sql = static_cast<std::string>(uv);
    BOOST_TEST(sql.find("id") != std::string::npos);
    BOOST_TEST(sql.find("age") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(update_returns_self)
{
    aquarius::update_view uv;
    auto& ref = uv(person{1, 25});
    BOOST_TEST(&ref == &uv);
}

BOOST_AUTO_TEST_CASE(update_with_grep_adds_where)
{
    aquarius::update_view uv;
    uv(person{1, 25});
    aquarius::grep_view<&person::id> g;
    g.equal(1);
    uv | g;
    std::string sql = static_cast<std::string>(uv);
    BOOST_TEST(sql.find("where") != std::string::npos);
    BOOST_TEST(sql.find("id = 1") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(update_with_two_grep_adds_and)
{
    aquarius::update_view uv;
    uv(person{1, 25});
    aquarius::grep_view<&person::id> g1;
    g1.equal(1);
    aquarius::grep_view<&person::age> g2;
    g2.equal(25);
    uv | g1;
    uv | g2;
    std::string sql = static_cast<std::string>(uv);
    BOOST_TEST(sql.find("where") != std::string::npos);
    BOOST_TEST(sql.find("and") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(update_overwrite_resets_sql)
{
    aquarius::update_view uv;
    uv(person{1, 25});
    uv(person{2, 30});
    std::string sql = static_cast<std::string>(uv);
    BOOST_TEST(sql.find("2") != std::string::npos);
    BOOST_TEST(sql.find("30") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()

// ===================================================================
// remove_view_tests
// ===================================================================
BOOST_AUTO_TEST_SUITE(tbl_remove_view_tests)

BOOST_AUTO_TEST_CASE(remove_generates_delete_sql)
{
    aquarius::remove_view rv{};
    rv(person{});
    std::string sql = static_cast<std::string>(rv);
    BOOST_TEST(sql == "delete from person");
}

BOOST_AUTO_TEST_CASE(remove_returns_self)
{
    aquarius::remove_view rv{};
    auto& ref = rv(person{});
    BOOST_TEST(&ref == &rv);
}

BOOST_AUTO_TEST_CASE(remove_overwrite_same_output)
{
    aquarius::remove_view rv{};
    rv(person{1, 25});
    rv(person{2, 30});
    std::string sql = static_cast<std::string>(rv);
    BOOST_TEST(sql == "delete from person");
}

BOOST_AUTO_TEST_CASE(remove_with_grep_adds_where)
{
    aquarius::remove_view rv{};
    rv(person{});
    aquarius::grep_view<&person::id> g;
    g.equal(1);
    rv | g;
    std::string sql = static_cast<std::string>(rv);
    BOOST_TEST(sql.find("delete from person") != std::string::npos);
    BOOST_TEST(sql.find("where") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(remove_with_two_grep_adds_and)
{
    aquarius::remove_view rv{};
    rv(person{});
    aquarius::grep_view<&person::id> g1;
    g1.equal(1);
    aquarius::grep_view<&person::age> g2;
    g2.equal(25);
    rv | g1;
    rv | g2;
    std::string sql = static_cast<std::string>(rv);
    BOOST_TEST(sql.find("where") != std::string::npos);
    BOOST_TEST(sql.find("and") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()

// ===================================================================
// select_view_tests
// ===================================================================
BOOST_AUTO_TEST_SUITE(tbl_select_view_tests)

BOOST_AUTO_TEST_CASE(select_all_generates_sql)
{
    aquarius::select_view<person> sv;
    std::string sql = static_cast<std::string>(sv);
    BOOST_TEST(sql == "select * from person");
}

BOOST_AUTO_TEST_CASE(select_constexpr_content)
{
    constexpr auto content = aquarius::select_view<person>::content;
    BOOST_TEST(content == "select * from person");
}

BOOST_AUTO_TEST_CASE(select_with_grep_adds_where)
{
    aquarius::select_view<person> sv;
    aquarius::grep_view<&person::id> g;
    g.equal(1);
    sv | g;
    std::string sql = static_cast<std::string>(sv);
    BOOST_TEST(sql.find("select * from person") != std::string::npos);
    BOOST_TEST(sql.find("where") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(select_with_multiple_grep_adds_and)
{
    aquarius::select_view<person> sv;
    aquarius::grep_view<&person::id> g1;
    g1.equal(1);
    aquarius::grep_view<&person::age> g2;
    g2.less(30);
    sv | g1;
    sv | g2;
    std::string sql = static_cast<std::string>(sv);
    BOOST_TEST(sql.find("where") != std::string::npos);
    BOOST_TEST(sql.find("and") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()

// ===================================================================
// transaction_tests
// ===================================================================
struct mock_service
{
    bool began = false;
    bool ended = false;
    void begin() { began = true; }
    void end() { ended = true; }
};

BOOST_AUTO_TEST_SUITE(tbl_remote_tests)

MYSQL_CONFIG_INVOKE(mysql)
{
    mysql.host = "localhost";
    mysql.port = 3306;
    mysql.db = "unittest";
    mysql.user = "root";
}

//BOOST_AUTO_TEST_CASE(connecting)
//{
//    person p{ 1, 1 };
//
//    aquarius::io_service_pool pool(1);
//
//    aquarius::module_router::get_mutable_instance().regist<aquarius::mysql_module>();
//
//    aquarius::module_router::get_mutable_instance().run(pool);
//
//    std::this_thread::sleep_for(2s);
//
//    auto future = aquarius::asio::co_spawn(pool.get_io_service(), [&] ()-> aquarius::asio::awaitable<void>
//                                           {
//                                               auto res = co_await(aquarius::insert(p) | aquarius::enter);
//
//                                               BOOST_TEST(res != 0);
//
//                                               auto select_res = co_await(aquarius::select<person> | aquarius::enter);
//
//                                               BOOST_TEST(select_res.size() != 0);
//
//                                           }, aquarius::asio::use_future);
//
//
//    std::thread t([&] { pool.run(); });
//
//    std::this_thread::sleep_for(2s);
//
//    future.get();
//
//    pool.stop();
//
//    t.join();
//}

BOOST_AUTO_TEST_SUITE_END()