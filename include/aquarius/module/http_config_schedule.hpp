#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/module/http_config_module.hpp>
#include <aquarius/module/schedule.hpp>

namespace aquarius
{
	inline auto mpc_http_version() -> awaitable<std::string>
	{
		co_return co_await mpc::call<std::string, http_config_module>(
			"http_config_module"sv,
			[](http_config_module* ptr) -> awaitable<std::string> { co_return ptr->get_version(); });
	}

	inline auto mpc_http_origin() -> awaitable<std::string>
	{
		co_return co_await mpc::call<std::string, http_config_module>(
			"http_config_module"sv,
			[](http_config_module* ptr) -> awaitable<std::string> { co_return ptr->get_origin(); });
	}
	inline auto mpc_http_root() -> awaitable<std::string>
	{
		co_return co_await mpc::call<std::string, http_config_module>(
			"http_config_module"sv,
			[](http_config_module* ptr) -> awaitable<std::string> { co_return ptr->get_root_dir(); });
	}
} // namespace aquarius