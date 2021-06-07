#pragma once
#include <cstdint>

namespace aquarius
{
	enum class proto_t
	{
		tcp,
		udp,
		http
	};

	struct tcp_header
	{
		uint32_t proto_id_;
		uint32_t part_id_;
		uint32_t reserve_;
	};

	struct request_header : public tcp_header
	{
		uint32_t src_id_;
		uint32_t session_id_;
	};

	struct response_header : public tcp_header
	{
		int32_t result_;
	};
}