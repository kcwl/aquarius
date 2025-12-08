#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>
#include <memory>

namespace aquarius
{
	template <typename T, typename Core>
	concept enable_pool_task = requires {
		typename T::return_type;

		{ std::declval<T>()(std::shared_ptr<Core>{}) } -> std::same_as<awaitable<typename T::return_type>>;
	};

	template <typename T, typename IoPool>
	class basic_pool
	{
		template <typename Task>
		struct initialize_task
		{
			initialize_task(std::shared_ptr<T> proc, Task&& t)
				: proc_ptr(proc)
				, task(std::forward<Task>(t))
			{}

			template <typename Self>
			auto operator()(Self& self, error_code ec = {}) -> awaitable<void>
			{
				
			}

			std::shared_ptr<T> proc_ptr;

			Task task;
		};

	public:
		explicit basic_pool()
			: index_(0)
			, task_pool_()
		{}

	public:
		template <typename... Args>
		auto run(std::size_t size, Args&&... args) -> awaitable<bool>
		{
			for (std::size_t i = 0; i < size; ++i)
			{
				auto ptr = std::make_shared<T>(std::forward<Args>(args)...);

				auto result = co_await ptr->start();

				if (!result)
					co_return false;

				task_pool_.emplace_back(ptr);
			}

			co_return true;
		}

		template <typename Task>
		requires(enable_pool_task<Task, T>)
		auto async_execute(Task&& task) -> awaitable<typename Task::return_type>
		{
			using return_type = typename Task::return_type;
			error_code ec{};

			auto proc_ptr = get_task_proc();

			if (!proc_ptr)
				co_return return_type{};

			co_return co_await task(proc_ptr);
		}

	private:
		std::shared_ptr<T> get_task_proc()
		{
			if (task_pool_.empty())
			{
				XLOG_WARNING() << "task pool is empty!";

				return nullptr;
			}
				

			if (index_ == task_pool_.size())
				index_ = 0;

			return task_pool_[index_++];
		}

	private:
		std::size_t index_;

		std::vector<std::shared_ptr<T>> task_pool_;
	};
} // namespace aquarius