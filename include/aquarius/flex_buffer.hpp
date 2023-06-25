#pragma once
#include <aquarius/detail/streambuf.hpp>
#include "../3rd/elastic/include/elastic.hpp"

namespace aquarius
{
	using flex_buffer_t = aquarius::detail::basic_streambuf<char, std::char_traits<char>>;
}