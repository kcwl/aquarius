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

	class http_config_module : public aquarius::_module<http_config_module>
	{
	public:
		http_config_module(const std::string& name)
			: _module<http_config_module>(name)
		{}

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

	inline auto mpc_http_version() -> asio::awaitable<std::string>
	{
		co_return co_await mpc::call<std::string, http_config_module>(
			[](http_config_module* ptr) -> asio::awaitable<std::string> { co_return ptr->get_version(); });
	}

	inline auto mpc_http_origin() -> asio::awaitable<std::string>
	{
		co_return co_await mpc::call<std::string, http_config_module>(
			[](http_config_module* ptr) -> asio::awaitable<std::string> { co_return ptr->get_origin(); });
	}
	inline auto mpc_http_root() -> asio::awaitable<std::string>
	{
		co_return co_await mpc::call<std::string, http_config_module>(
			[](http_config_module* ptr) -> asio::awaitable<std::string> { co_return ptr->get_root_dir(); });
	}
} // namespace aquarius