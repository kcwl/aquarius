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

		enum class read_handle_result
		{
			ok = 1,
			error = 2,
			waiting_for_query = 3
		};
	} // namespace impl
} // namespace aquarius