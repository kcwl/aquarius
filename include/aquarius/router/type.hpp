#pragma once
#include <cstdint>

namespace aquarius
{
	enum class router_type : int32_t
	{
		basic_router = 0,
		reverse,
	};
} // namespace aquarius