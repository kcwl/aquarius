#pragma once
#include <aquarius/virgo/http_version.hpp>
#include <string>

namespace
{
	constexpr static aquarius::http_version global_http_version = aquarius::http_version::http1_1;
}

namespace aquarius
{
	inline std::string make_request_command_header()
	{}

	inline std::string make_response_command_header(http_status s)
	{
		return std::format("{} {} {}\r\n", version_to_string(global_http_version), static_cast<int32_t>(s),
						   status_to_string(s));
	}

	
} // namespace aquarius