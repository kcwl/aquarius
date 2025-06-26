#pragma once
#include <cstdint>

namespace aquarius
{
	struct local_header
	{
		uint32_t context_id;
		uint64_t rpc_id;
		uint8_t mode;
		uint64_t length;
		uint32_t crc;
		int64_t timestamp;
	};
} // namespace aquarius