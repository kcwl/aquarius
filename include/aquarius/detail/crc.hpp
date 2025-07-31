#pragma once
#include <boost/crc.hpp>

namespace aquarius
{
	namespace detail
	{
		inline uint32_t crc32(const char* data, std::size_t bytes)
		{
			boost::crc_32_type result{};
			result.process_bytes(data, bytes);

			return result.checksum();
		}
	} // namespace detail
} // namespace aquarius