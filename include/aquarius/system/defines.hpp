#pragma once
#include <chrono>
#include <cstdint>

using namespace std::chrono_literals;

namespace aquarius
{
	constexpr static auto heart_time_interval = 10s;

	struct http
	{};

	struct tcp
	{};

	enum class conn_mode
	{
		client,
		server
	};

	enum class ssl_mode
	{
		nossl,
		ssl
	};

} // namespace aquarius