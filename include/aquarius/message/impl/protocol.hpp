#pragma once
#include <cstdint>

namespace aquarius
{
	constexpr static std::size_t unknown_req_number = 0;
	constexpr static std::size_t unknown_resp_number = 1;
	constexpr static std::size_t message_base_number = 1000;

	constexpr static std::size_t http_number = static_cast<std::size_t>(-1);  
} // namespace aquarius