#pragma once
#include <string>
#include <mutex>
#include <aquarius/tag_invoke.hpp>

namespace aquarius
{
	struct http_param
	{
		std::string root_dir;
	};

	struct http_param_tag
	{};

	inline std::string get_www_root_dir()
	{
		static http_param param{};

		static std::once_flag once{};

		std::call_once(once, [&] 
					   {
						   tag_invoke(http_param_tag{}, param);
					   });

		return param.root_dir;
	}
} // namespace aquarius