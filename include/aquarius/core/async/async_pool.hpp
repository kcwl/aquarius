#pragma once
#include <aquarius/core/singleton.hpp>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

namespace aquarius
{
	class async_pool : public aquarius::singleton<async_pool>
	{
	protected:
		async_pool()
			: task_mutex_()
			, task_queue_()
			, thread_mutex_()
			, cv_()
			, threads_()
			, stop_(false)
		{
			set_thread(std::thread::hardware_concurrency());
		}

		~async_pool()
		{
			this->stop_.store(true);

			for (auto& t : threads_)
			{
				if (!t && !t->joinable())
					continue;

				t->join();
			}
		}

	public:
		void post(const std::function<void()>& func)
		{
			std::lock_guard lk(task_mutex_);

			task_queue_.push(func);

			cv_.notify_all();
		}

	private:
		void set_thread(std::size_t number)
		{
			for (std::size_t i = 0; i < number; ++i)
			{
				threads_.push_back(std::make_shared<std::thread>(
					[&]
					{
						std::unique_lock lk(thread_mutex_);

						cv_.wait(lk, [&] { return this->stop_.load() || !this->task_queue_.empty(); });

						this->execute();
					}));
			}
		}

		void execute()
		{
			std::lock_guard lk(task_mutex_);

			auto& task = task_queue_.front();

			try
			{
				task();
			}
			catch (...) {}

			task_queue_.pop();
		}

	private:
		std::mutex task_mutex_;

		std::queue<std::function<void()>> task_queue_;

		std::mutex thread_mutex_;

		std::condition_variable cv_;

		std::vector<std::shared_ptr<std::thread>> threads_;

		std::atomic_bool stop_;
	};
} // namespace aquarius