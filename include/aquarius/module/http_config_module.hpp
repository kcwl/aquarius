#pragma once
#include <aquarius/basic_module.hpp>
#include <aquarius/module/module_register.hpp>

namespace aquarius
{
	struct http_param
	{
		bool enable()
		{
			return true;
		}

		std::string root_dir;
		std::string control_allow_origin;
		// virgo::http_version version = virgo::http_version::http1_1;
		std::string version;
	};

	AQUARIUS_MODULE(http_config_module)
	{
	public:
		http_config_module() = default;

	public:
		std::string get_version() const
		{
			return config_.version;
		}

		std::string get_origin() const
		{
			return config_.control_allow_origin;
		}

		std::string get_root_dir() const
		{
			return config_.root_dir;
		}

	private:
		http_param config_;
	};
} // namespace aquarius