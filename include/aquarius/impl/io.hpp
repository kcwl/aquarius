#pragma once
#include <elastic.hpp>

namespace aquarius
{
	using flex_buffer_t = elastic::serialize_streambuf<char,std::char_traits<char>>;
}