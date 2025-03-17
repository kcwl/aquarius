#pragma once
#include <aquarius/serialize/detail/flex_buffer.hpp>

namespace aquarius
{
	using flex_buffer_t = flex_buffer<uint8_t, std::char_traits<uint8_t>>;
} // namespace elastic