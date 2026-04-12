#pragma once
#include <aquarius/module/mysql_module.hpp>
#include <aquarius/module/schedule.hpp>

namespace aquarius
{
	struct enter_view
	{
		template <typename T>
		auto select(const std::string& sql) const -> asio::awaitable<std::vector<T>>
		{
			co_return co_await mpc_async_call<&mysql_module::async_query<T>>(std::string_view(sql));
		}

		auto insert(const std::string& sql) const -> asio::awaitable<std::size_t>
		{
			co_return co_await mpc_async_call<&mysql_module::async_execute>(std::string_view(sql));
		}

		auto remove(const std::string& sql) const -> asio::awaitable<std::size_t>
		{
			co_return co_await mpc_async_call<&mysql_module::async_execute>(std::string_view(sql));
		}

		auto update(const std::string& sql) const -> asio::awaitable<std::size_t>
		{
			co_return co_await mpc_async_call<&mysql_module::async_execute>(std::string_view(sql));
		}
	};

	inline enter_view enter;
} // namespace aquarius