#pragma once
#include <string>
#include <mutex>
#include <aquarius/tag_invoke.hpp>
#include <aquarius/virgo/http_version.hpp>

namespace aquarius
{
	struct http_param
	{
		std::string root_dir;
		std::string control_allow_origin;
		virgo::http_version version = virgo::http_version::http1_1;
	};

	struct http_param_tag
	{};

	inline http_param get_http_param()
	{
		static http_param param{};

		static std::once_flag once{};

		std::call_once(once, [&]
					   {
						   value_to<http_param_tag>(param);
					   });

		return param;
	}
} // namespace aquarius