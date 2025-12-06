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
				auto result = co_await task(proc_ptr);

				self.complete(ec, result);
			}

			std::shared_ptr<T> proc_ptr;

			Task task;
		};

	public:
		explicit basic_pool(IoPool& io_pool)
			: io_pool_(io_pool)
		{}

	public:
		template <typename... Args>
		void run(Args&&... args)
		{
			auto size = io_pool_.size();

			for (std::size_t i = 0; i < size; ++i)
			{
				task_pool_.emplace_back(std::make_shared<T>(io_pool_.get_io_service(), std::forward<Args>(args)...));
			}
		}

		template <typename Task>
		requires(enable_pool_task<Task, T>)
		auto async_execute(Task&& task) -> awaitable<typename Task::return_type>
		{
			error_code ec{};

			auto result = co_await async_execute_impl(std::forward<Task>(task), redirect_error(use_awaitable, ec));

			if (ec)
			{
				using return_type = typename Task::return_type;

				co_return return_type{};
			}

			co_return result;
		}

		void stop()
		{
			io_pool_.stop();
		}

	private:
		std::shared_ptr<T> get_task_proc()
		{
			if (task_pool_.empty())
				return nullptr;

			if (index_ == task_pool_.size())
				index_ = 0;

			return task_pool_[index_++];
		}

		template <typename Task, typename CompleteToken>
		auto async_execute_impl(Task&& task, CompleteToken&& token) -> awaitable<typename Task::return_type>
		{
			auto task_proc = get_task_proc();

			using return_type = typename Task::return_type;

			co_return co_await async_compose<CompleteToken, void(error_code, return_type)>(
				initialize_task(task_proc, std::forward<Task>(task)), token, io_pool_.get_io_service());
		}

	private:
		IoPool& io_pool_;

		std::size_t index_;

		std::vector<std::shared_ptr<T>> task_pool_;
	};
} // namespace aquarius