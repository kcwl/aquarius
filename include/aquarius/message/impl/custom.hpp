#pragma once
#include <cstdint>

namespace aquarius
{
	struct custom_tcp_request_header
	{
		uint32_t session_id;
	};

	struct custom_tcp_response_header
	{
		uint32_t result;
	};
} // namespace aquarius