#pragma once
#include <cstdint>

namespace aquarius
{
	namespace impl
	{
		constexpr static int32_t heart_time_interval = 10;

		enum class visit_mode
		{
			input,
			output
		};

		struct ssl_socket
		{};
	} // namespace impl
} // namespace aquarius