#pragma once
#include <chrono>
#include <cstdint>

using namespace std::chrono_literals;

namespace aquarius
{
	constexpr static auto heart_time_interval = 10s;

	enum class visit_mode
	{
		input,
		output
	};

	struct ssl_socket
	{};

	struct connect_tcp
	{};

	struct http
	{};

	enum class read_handle_result
	{
		ok = 1,
		unknown_error,
		unknown_proto,
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