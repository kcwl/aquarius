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

	enum class read_handle_result
	{
		failed,
		ok,
		unknown_error,
		unknown_proto,
		unknown_ctx,
		header_error,
		body_error,
		context_error,
		waiting_for_query,
		null_message,
		reset_peer,
		report
	};

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