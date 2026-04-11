#pragma once
#include <string>
#include <ranges>
#include <aquarius/resource/config_tag_invoke.hpp>

namespace aquarius
{
	struct http_config
	{
		std::string root_dir;
		std::string control_allow_origin;
		std::string control_allow_methods;
		std::string control_allow_headers;
		std::string control_max_age;
		bool control_allow_credentials;
		// virgo::http_version version = virgo::http_version::http1_1;
		std::string version;

		bool check_origin(std::string_view origin)
		{
			std::string_view origin_view(control_allow_origin);

			auto op = origin_view | std::views::split(',');

			auto iter = std::ranges::find_if(op.begin(), op.end(), [&] (auto o)
											  {
												  return std::string_view(o) == origin;
											  });

			return iter != op.end();
		}

		bool check_method(std::string_view methods)
		{
			if (methods.empty())
				return false;

			auto op = methods | std::views::split(',');

			for (const auto& o : op)
			{
				if (!control_allow_methods.contains(std::string_view(o)))
				{
					return false;
				}
			}

			return true;
		}

		bool check_headers(std::string_view headers)
		{
			if (headers.empty())
				return false;

			auto op = headers | std::views::split(',');

			for (const auto& o : op)
			{
				if (!control_allow_headers.contains(std::string_view(o)))
				{
					return false;
				}
			}

			return true;
		}
	};
} // namespace aquarius

#define HTTP_CONFIG_INVOKE(cfg) CONFIG_MICRO(aquarius::http_config, cfg)