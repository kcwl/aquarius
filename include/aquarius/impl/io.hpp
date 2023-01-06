#pragma once
#include <elastic.hpp>

namespace aquarius
{
	namespace impl
	{
		using flex_buffer_t = elastic::serialize_streambuf<char, std::char_traits<char>>;
	}
} // namespace aquarius