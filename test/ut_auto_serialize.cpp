#include <boost/test/unit_test.hpp>

#include <aquarius/serialize/binary.hpp>
#include <aquarius/detail/flex_buffer.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_CASE(integer_roundtrip)
{
    binary_parse p;
    flex_buffer buff;

    p.to_datas<int>(123456, buff);

    auto v = p.from_datas<int>(buff);
    BOOST_CHECK_EQUAL(v, 123456);
}

BOOST_AUTO_TEST_CASE(zigzag_roundtrip)
{
    binary_parse p;
    flex_buffer buff;

    p.to_datas<int64_t>(-42, buff);
    auto v = p.from_datas<int64_t>(buff);
    BOOST_CHECK_EQUAL(v, -42);
}

BOOST_AUTO_TEST_CASE(pod_and_string_roundtrip)
{
    binary_parse p;
    flex_buffer buff;

    double d = 3.14159;
    p.to_datas<double>(d, buff);
    auto d2 = p.from_datas<double>(buff);
    BOOST_CHECK_CLOSE(d, d2, 1e-6);

    std::string s = "serialize-test";
    p.to_datas<std::string>(s, buff);
    auto s2 = p.from_datas<std::string>(buff);
    BOOST_CHECK_EQUAL(s2, s);
}

BOOST_AUTO_TEST_CASE(vector_and_map_roundtrip)
{
    binary_parse p;
    flex_buffer buff;

    std::vector<int> v = {1,2,3,4,5};
    p.to_datas<std::vector<int>>(v, buff);
    auto v2 = p.from_datas<std::vector<int>>(buff);
    BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(), v2.begin(), v2.end());

    std::map<int,std::string> m = {{1,"a"},{2,"b"}};
    p.to_datas<std::map<int,std::string>>(m, buff);
    auto m2 = p.from_datas<std::map<int,std::string>>(buff);
    BOOST_CHECK_EQUAL(m2.size(), m.size());
}
