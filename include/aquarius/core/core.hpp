#pragma once
#include <chrono>
#include <cstdint>

using namespace std::chrono_literals;

namespace aquarius
{
	constexpr static auto heart_time_interval = 10s;

	constexpr static auto timeout_dura = 30ms;

	constexpr static auto pack_limit = 65536;

	constexpr static size_t default_proto = 0;

	struct http
	{};

	struct tcp
	{};

	enum class conn_mode
	{
		client,
		basic_server
	};

	enum class ssl_mode
	{
		nossl,
		ssl
	};

	enum class pack_flag
	{
		normal = 0x01,
		middle = 0x10,
		end = 0x11
	};

} // namespace aquarius