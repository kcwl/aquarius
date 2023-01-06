#pragma once
#include <cstdint>

namespace aquarius
{
	inline static constexpr int buffer_capacity = 4096;

	enum class io_state
	{
		read, 
		write
	};
}