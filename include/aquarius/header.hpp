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

	class proto_header
	{
	public:
		proto_t  proto_type_;
		uint32_t proto_id_;
		uint32_t session_id_;
	};
}