#pragma once
#include <cstdint>
#include "filed.hpp"

namespace  aquarius
{
	namespace tcp
	{
		struct header_value
		{
			uint32_t type_;
			uint32_t length_;
			uint32_t proto_;
			uint32_t result_;
		};

		using header_fields = basic_fileds<std::allocator<header_value>>;
	}
}