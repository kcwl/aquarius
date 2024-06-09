#pragma once
#include <chrono>
#include <cstdint>

using namespace std::chrono_literals;

namespace aquarius
{
	constexpr static auto heart_time_interval = 10s;

	constexpr static auto timeout_dura = 1s;

	constexpr static auto deadline_dura = 1ms;
} // namespace aquarius