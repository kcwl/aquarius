#pragma once
#include <aquarius/module/module.hpp>
#include <aquarius/module/schedule.hpp>

namespace aquarius
{
	struct http_param
	{
		constexpr static auto path = "http"sv;

		bool enable()
		{
			return true;
		}

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

	inline auto mpc_http_version() -> awaitable<std::string>
	{
		co_return co_await mpc::call<std::string, http_config_module>(
			[](http_config_module* ptr) -> awaitable<std::string> { co_return ptr->get_version(); });
	}

	inline auto mpc_http_origin() -> awaitable<std::string>
	{
		co_return co_await mpc::call<std::string, http_config_module>(
			[](http_config_module* ptr) -> awaitable<std::string> { co_return ptr->get_origin(); });
	}
	inline auto mpc_http_root() -> awaitable<std::string>
	{
		co_return co_await mpc::call<std::string, http_config_module>(
			[](http_config_module* ptr) -> awaitable<std::string> { co_return ptr->get_root_dir(); });
	}
} // namespace aquarius