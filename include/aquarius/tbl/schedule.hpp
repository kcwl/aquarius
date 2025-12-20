#pragma once
#include <aquarius/module/schedule.hpp>
#include <aquarius/tbl/sql_connector.hpp>
#include <aquarius/tbl/task.hpp>

namespace aquarius
{
	namespace tbl
	{
		template <typename R, typename T>
		auto schedule_insert(const std::string& module_name, T&& v) -> awaitable<R>
		{
			co_return co_await module_schedule<sql_connector>(
				module_name, std::make_shared<sql_task<insert, T, R>>(std::forward<T>(v)));
		}

		template <typename R, typename T>
		auto schedule_delete(const std::string& module_name, T&& v) -> awaitable<R>
		{
			co_return co_await module_schedule<sql_connector>(
				module_name, std::make_shared<sql_task<delete_, T, R>>(std::forward<T>(v)));
		}

		template <typename R, typename T>
		auto schedule_update(const std::string& module_name, T&& v) -> awaitable<R>
		{
			co_return co_await module_schedule<sql_connector>(
				module_name, std::make_shared<sql_task<update, T, R>>(std::forward<T>(v)));
		}

		template <typename R>
		auto schedule_select(const std::string& module_name) -> awaitable<R>
		{
			co_return co_await module_schedule<sql_connector>(module_name,
															  std::make_shared<sql_task<select_, int, R>>(0));
		}

		// template <typename R, typename T>
		// auto schedule_create(const std::string& module_name, T&& v) -> awaitable<R>
		//{
		//	co_return co_await module_schedule(module_name, std::make_shared<sql_task<create, T,
		//R>>(std::forward<T>(v)));
		// }

		template <typename R = std::size_t>
		auto schedule_query(const std::string& module_name, std::string_view sql) -> awaitable<R>
		{
			co_return co_await module_schedule<sql_connector>(module_name,
															  std::make_shared<sql_str_task<select_, R>>(sql));
		}

		template <typename R = std::size_t>
		auto schedule_execute(const std::string& module_name, std::string_view sql) -> awaitable<R>
		{
			co_return co_await module_schedule<sql_connector>(module_name,
															  std::make_shared<sql_str_task<insert, R>>(sql));
		}
	} // namespace tbl
} // namespace aquarius