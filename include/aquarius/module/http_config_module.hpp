#pragma once
#include <aquarius/module/module.hpp>

namespace aquarius
{
	struct http_param
	{
		constexpr static auto path = "http"sv;

		std::string root_dir;
		std::string control_allow_origin;
		// virgo::http_version version = virgo::http_version::http1_1;
		std::string version;
	};

	class http_config_module : public aquarius::_module<http_config_module, aquarius::http_param>
	{
	public:
		http_config_module(const std::string& name)
			: _module<http_config_module, aquarius::http_param>(name)
		{}

	public:
		std::string get_version() const
		{
			return configs().version;
		}

		std::string get_origin() const
		{
			return configs().control_allow_origin;
		}

		std::string get_root_dir() const
		{
			return configs().root_dir;
		}
	};
} // namespace aquarius