#pragma once
#include <elastic.hpp>

namespace aquarius
{
	namespace impl
	{
		using flex_buffer_t = elastic::serialize_streambuf<char, std::char_traits<char>>;

		using oarchive = elastic::binary_oarchive;

		using iarchive = elastic::binary_iarchive;
	} // namespace impl
} // namespace aquarius