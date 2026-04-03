#pragma once
#include <aquarius/module/sql_module.hpp>

namespace aquarius
{
	struct enter_view
	{
		template<typename T>
		auto select(const std::string& sql) ->asio::awaitable<std::vector<T>>
		{
			co_return co_await mpc_select<T>(sql);
		}

		auto insert(const std::string& sql) ->asio::awaitable<std::size_t>
		{
			co_return co_await mpc_insert(sql);
		}

		auto remove(const std::string& sql) ->asio::awaitable<std::size_t>
		{
			co_return co_await mpc_delete(sql);
		}

		auto update(const std::string& sql) ->asio::awaitable<std::size_t>
		{
			co_return co_await mpc_update(sql);
		}
	};

	inline enter_view enter;
} // namespace aquarius