#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/detail/flex_buffer.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_serialize_error_paths)

BOOST_AUTO_TEST_CASE(integer_eof_throws)
{
    binary_parse bp;
    aquarius::flex_buffer b;

    // write EOF marker byte to trigger early out_of_range
    b.sputc(static_cast<char>(aquarius::flex_buffer::traits_type::eof()));

    BOOST_CHECK_THROW(bp.from_datas<int>(b), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(pod_and_string_insufficient_throws)
{
    binary_parse bp;
    aquarius::flex_buffer b;

    // pod insufficient: write only 2 bytes for a double
    b.sputn("ab", 2);
    BOOST_CHECK_THROW(bp.from_datas<double>(b), std::out_of_range);

    // string: write length 5, then only 2 bytes
    aquarius::flex_buffer b2;
    bp.to_datas<std::size_t>(5, b2);
    b2.sputn("xy", 2);
    BOOST_CHECK_THROW(bp.from_datas<std::string>(b2), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(repeated_insufficient_throws)
{
    binary_parse bp;
    aquarius::flex_buffer b;

    // declare vector size 1 but provide no element data
    bp.to_datas<std::size_t>(1, b);

    BOOST_CHECK_THROW(bp.from_datas<std::vector<int>>(b), std::out_of_range);
}

BOOST_AUTO_TEST_SUITE_END()
