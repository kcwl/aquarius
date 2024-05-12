#pragma once
#include <cstdint>
#include <aquarius/core/elastic.hpp>

namespace aquarius
{
	struct custom_tcp_request_header
	{
		uint32_t session_id;

		ELASTIC_ACCESS(session_id);
	};

	struct custom_tcp_response_header : custom_tcp_request_header
	{
		uint32_t result;

		ELASTIC_ACCESS_IF(custom_tcp_request_header, result);
	};
} // namespace aquarius