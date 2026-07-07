#pragma once
#include <aquarius.hpp>

namespace aquarius
{
	namespace serviced
	{
		namespace
		{
			uint64_t make_host_and_port(const std::string& host, int32_t port)
			{
				uint64_t host_u = asio::ip::address(asio::ip::make_address(host)).to_v4().to_uint();

				return (host_u << 32) | port;
			}
		} // namespace
	} // namespace serviced
} // namespace aquarius