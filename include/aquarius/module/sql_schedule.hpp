#pragma once
#include <aquarius/module/schedule.hpp>
#include <aquarius/module/sql_module.hpp>

namespace aquarius
{
	template <typename T>
	auto mpc_insert(T&& v) -> awaitable<std::size_t>
	{
		co_return co_await mpc_execute(tbl::insert_view(std::forward<T>(v))());
	}

	template <typename T>
	auto mpc_delete(T&& v) -> awaitable<std::size_t>
	{
		co_return co_await mpc_execute(tbl::remove_view<T>(std::forward<T>(v))());
	}

	template <typename T>
	auto mpc_update(T&& v) -> awaitable<std::size_t>
	{
		co_return co_await mpc_execute(tbl::update_view(std::forward<T>(v))());
	}

	template <typename T>
	auto mpc_select() -> awaitable<std::vector<T>>
	{
		co_return co_await mpc_execute(tbl::select_view<T>()());
	}

	template <typename T>
	auto mpc_query(std::string_view sql) -> awaitable<std::vector<T>>
	{
		co_return co_await mpc::call<std::vector<T>, sql_module>(
			"sql_module", [&](sql_module* ptr) { co_return co_await ptr->template async_query<T>(sql); });
	}

	auto mpc_execute(std::string_view sql) -> awaitable<std::size_t>
	{
		co_return co_await mpc::call<std::size_t, sql_module>("sql_module",
															  [&](sql_module* ptr) -> awaitable<std::size_t>
															  { co_return co_await ptr->async_execute(sql); });
	}
} // namespace aquarius