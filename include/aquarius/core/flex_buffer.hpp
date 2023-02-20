#pragma once
#include <elastic.hpp>

namespace aquarius
{
	namespace core
	{
		using flex_buffer_t = elastic::streambuf<char, std::char_traits<char>>;

		using oarchive = elastic::binary_oarchive;

		using iarchive = elastic::binary_iarchive;
	} // namespace impl
} // namespace aquarius