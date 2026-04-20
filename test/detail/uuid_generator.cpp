#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

#include "aquarius/detail/uuid_generator.hpp"

#include <unordered_set>

using aquarius::detail::uuid_generator;

BOOST_AUTO_TEST_CASE(unique_values)
{
    uuid_generator gen;
    const std::size_t N = 1000;
    std::unordered_set<uint32_t> seen;
    seen.reserve(N * 2);

    for (std::size_t i = 0; i < N; ++i)
    {
        uint32_t v = gen();
        seen.insert(v);
    }

    BOOST_TEST(seen.size() == N);
}
