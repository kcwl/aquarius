#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/trie.hpp>


BOOST_AUTO_TEST_SUITE(tries)

using namespace std::string_view_literals;

BOOST_AUTO_TEST_CASE(tri)
{
	aquarius::detail::trie<std::function<void()>> normal_tre{};

	std::function<void()> leaf = [] () { std::cout << "hello leaf;"; };

	normal_tre.add("11001"sv, leaf);

	normal_tre.add("11"sv, leaf);

	BOOST_CHECK(normal_tre.find("11"sv) != nullptr);

	normal_tre.remove("11001"sv);
	normal_tre.remove("11"sv);

	auto cur = normal_tre.find("11001"sv);

	BOOST_TEST(!cur);
}


BOOST_AUTO_TEST_SUITE_END()