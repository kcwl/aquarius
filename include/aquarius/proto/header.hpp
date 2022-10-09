#pragma once
#include <cstdint>

namespace aquarius
{
	struct tcp_header
	{
		int32_t size_;
	};

	struct tcp_request_header : tcp_header
	{
		uint32_t uid_;
	};

	struct tcp_response_header : tcp_header
	{
		int32_t result_;
	};
}