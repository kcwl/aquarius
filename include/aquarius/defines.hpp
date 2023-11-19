#pragma once
#include <chrono>
#include <cstdint>

using namespace std::chrono_literals;

namespace aquarius
{
	constexpr static auto heart_time_interval = 10s;

	struct ssl_socket
	{};

	struct connect_tcp
	{};

	struct http
	{};

	struct tcp
	{};

	enum class read_handle_result
	{
		ok = 1,
		unknown_error,
		unknown_proto,
		unknown_ctx,
		header_error,
		body_error,
		context_error,
		waiting_for_query,
		null_message
	};

	enum class connect_event
	{
		start,
		close
	};

} // namespace aquarius