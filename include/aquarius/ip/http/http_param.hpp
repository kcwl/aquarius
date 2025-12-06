#pragma once
#include <aquarius/virgo/http_version.hpp>
#include <string>

namespace aquarius
{
	struct http_param
	{
		std::string root_dir;
		std::string control_allow_origin;
		virgo::http_version version = virgo::http_version::http1_1;
	};

	inline http_param get_http_param()
	{
		static http_param param{};

		return param;
	}
} // namespace aquarius