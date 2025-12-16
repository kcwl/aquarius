#pragma once
#include <aquarius/module/schedule.hpp>

namespace aquarius
{
	namespace tbl
	{
		template <typename Tag, typename T, typename R = std::size_t>
		auto sql_schedule(std::string_view module_name, T&& v) -> awaitable<R>
		{
			co_return co_await module_schadule(module_name, sql_task<Tag, T, R>(std::forward<T>(v)));
		}

		template <typename T, typename R = std::size_t>
		auto sql_schedule(std::string_view module_name, std::string_view sql) -> awaitable<R>
		{
			co_return co_await module_schadule(module_name, sql_str_task<T, R>(sql));
		}
	} // namespace tbl
} // namespace aquarius