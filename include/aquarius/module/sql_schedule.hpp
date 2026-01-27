#pragma once
#include <aquarius/module/schedule.hpp>
#include <aquarius/module/sql_module.hpp>
#include <aquarius/tbl/engine/generate_view.hpp>

namespace aquarius
{
	template <typename T>
	inline auto mpc_insert(T&& v) -> awaitable<std::size_t>
	{
		co_return co_await mpc_execute(tbl::insert(std::forward<T>(v)));
	}

	template <typename T>
	inline auto mpc_delete(T&& v) -> awaitable<std::size_t>
	{
		co_return co_await mpc_execute(tbl::remove(std::forward<T>(v)));
	}

	template <typename T>
	inline auto mpc_update(T&& v) -> awaitable<std::size_t>
	{
		co_return co_await mpc_execute(tbl::update(std::forward<T>(v)));
	}

	template <typename T>
	inline auto mpc_select(T&& v) -> awaitable<std::vector<T>>
	{
		co_return co_await mpc_execute(tbl::select(std::forward<T>(v)));
	}

	template <typename T>
	inline auto mpc_query(std::string_view sql) -> awaitable<std::vector<T>>
	{
		co_return co_await mpc::call<std::vector<T>, sql_module>(
			"sql_module", [&](sql_module* ptr) -> awaitable<std::vector<T>>
			{ 
				co_return co_await ptr->template async_query<T>(sql); 
			});
	}

	inline auto mpc_execute(std::string_view sql) -> awaitable<std::size_t>
	{
		co_return co_await mpc::call<std::size_t, sql_module>("sql_module",
															  [&](sql_module* ptr) -> awaitable<std::size_t>
															  { co_return co_await ptr->async_execute(sql); });
	}

	inline auto mpc_execute(const std::vector<std::string>& sqls) -> awaitable<bool>
	{
		co_return co_await mpc::call<std::size_t, sql_module>("sql_module",
															  [&](sql_module* ptr) -> awaitable<bool>
															  { co_return co_await ptr->async_execute(sqls); });
	}
} // namespace aquarius