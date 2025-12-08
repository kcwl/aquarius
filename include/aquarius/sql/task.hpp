#pragma once
#include <aquarius/asio.hpp>
#include <memory>
#include <string_view>

namespace aquarius
{
	enum class sql_tag
	{
		query,
		execute
	};

	template <auto Tag, typename T>
	struct sql_task
	{
		sql_task(std::string_view sql)
			: sql_str_(sql)
		{}

		using return_type = T;

		template <typename Core>
		auto operator()(std::shared_ptr<Core> task_proc) -> awaitable<return_type>
		{
			if (!task_proc)
				co_return return_type{};

			if constexpr (Tag == sql_tag::query)
			{
				co_return co_await task_proc->template async_query<return_type>(sql_str_);
			}
			else
			{
				co_return co_await task_proc->async_execute(sql_str_);
			}
		}

		std::string sql_str_;
	};
} // namespace aquarius