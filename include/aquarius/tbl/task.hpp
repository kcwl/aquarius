#pragma once
#include <aquarius/asio.hpp>
#include <memory>
#include <string_view>

namespace aquarius
{
	namespace tbl
	{
		struct create
		{};
		struct select_
		{};
		struct update
		{};
		struct insert
		{};
		struct delete_
		{};

		template <typename Tag, typename T, typename R>
		struct sql_task
		{
			sql_task(T&& value)
				: value_(value)
			{}

			using return_type = R;

			template <typename Core>
			auto operator()(std::shared_ptr<Core> task_proc) -> awaitable<return_type>
			{
				if (!task_proc)
					co_return return_type{};

				if constexpr (std::same_as<Tag, select_>)
				{
					constexpr auto sql_str = tbl::select_view<T>();

					co_return co_await task_proc->template async_query<return_type>(sql_str);
				}
				else if constexpr (std::same_as<Tag, update>)
				{
					co_return co_await task_proc->template async_execute<return_type>(tbl::update_view(value_));
				}
				else if constexpr (std::same_as<Tag, insert>)
				{
					co_return co_await task_proc->template async_execute<return_type>(tbl::insert_view(value_));
				}
				else if constexpr (std::same_as<Tag, delete_>)
				{
					co_return co_await task_proc->template async_execute<return_type>(tbl::remove_view<T>(value_));
				}
				else if constexpr (std::same_as<Tag, create>)
				{
					co_return co_await task_proc->template async_execute<return_type>(tbl::create_view<T>());
				}
				else
				{
					static_assert(false, "Invalid operation type");
				}
			}

			T value_;
		};

		template <typename T, typename R>
		struct sql_str_task
		{
			sql_str_task(std::string_view sql)
				: sql_(sql)
			{}

			using return_type = R;

			template <typename Core>
			auto operator()(std::shared_ptr<Core> task_proc) -> awaitable<return_type>
			{
				if (!task_proc)
					co_return return_type{};

				if constexpr (std::same_as<T, select_>)
				{
					co_return co_await task_proc->template async_query<return_type>(sql_);
				}
				else
				{
					co_return co_await task_proc->template async_execute<return_type>(sql_);
				}
			}

			std::string sql_;
		};
	} // namespace tbl
} // namespace aquarius