#pragma once
#include <cstdint>
#include <chrono>

using namespace std::chrono_literals;

namespace aquarius
{
	namespace impl
	{
		constexpr static auto heart_time_interval = 10s;

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