#pragma once
#include <cstdint>

namespace aquarius
{
	class tcp_request_header
	{
	public:
		tcp_request_header() = default;

	public:
		uint64_t uuid_;
	};

	class tcp_response_header : public tcp_request_header
	{
	public:
		tcp_response_header() = default;

	public:
		int result_;
	};
} // namespace aquarius