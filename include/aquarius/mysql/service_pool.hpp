#pragma once
#include <algorithm>
#include <aquarius/io_service_pool.hpp>
#include <deque>
#include <format>
#include <memory>
#include <thread>
#include <vector>

namespace aquarius
{
	template <typename _Service>
	class service_pool
	{
		using service_ptr = std::shared_ptr<_Service>;

	public:
		template <typename... _Args>
		explicit service_pool(io_service_pool& pool, std::size_t connects, _Args&&... args)
			: pools_(pool)
			, has_stop_(false)
			, conn_num_(connects)
		{
			make_service_pool(pools_, std::forward<_Args>(args)...);
		}

		~service_pool() = default;

		void start()
		{
			for (auto& s : services_)
			{
				s->run();
			}
		}

		void stop()
		{
			for (auto& s : services_)
			{
				s->close();
			}
		}

		std::future<bool> async_execute(const std::string& sql)
		{
			std::promise<bool> promise{};
			auto future = promise.get_future();

			auto conn_ptr = get_service();

			if (conn_ptr != nullptr)
			{
				conn_ptr->template async_excute(sql, [&](bool value) { promise.set_value(value); });
			}
			else
			{
				auto f = [&](service_ptr conn_ptr)
				{ conn_ptr->template async_excute(sql, [&](bool value) { promise.set_value(value); }); };

				push_queue(std::move(f));
			}

			return future;
		}

		template <typename T, typename F, typename... Args>
		auto async_pquery(F&& f, Args&&... args)
		{
			return async_query<T>(std::format(std::forward<F>(f), std::forward<Args>(args)...));
		}

		template <typename _Ty>
		auto async_query(const std::string& sql)
		{
			std::promise<_Ty> promise{};

			auto future = promise.get_future();

			auto conn_ptr = get_service();

			if (conn_ptr != nullptr)
			{
				return conn_ptr->template async_query(sql, [&](_Ty&& value) { promise.set_value(std::move(value)); });
			}
			else
			{
				auto f = [&](service_ptr conn_ptr)
				{ conn_ptr->template async_query(sql, [&](_Ty&& value) { promise.set_value(std::move(value)); }); };

				push_queue(std::move(f));
			}

			return {};
		}

	private:
		template <typename... _Args>
		void make_service_pool(io_service_pool& pool, _Args&&... args)
		{
			for (std::size_t i = 0; i < conn_num_; i++)
			{
				services_.push_back(std::make_shared<_Service>(pool.get_io_service(), std::forward<_Args>(args)...));
			}

			thread_ptr_.reset(new std::thread(
				[&]
				{
					while (!has_stop_.load())
					{
						std::lock_guard lk(mutex_);

						if (queue_.empty())
							continue;

						auto& task = queue_.front();

						task();

						queue_.pop_front();
					}
				}));
		}

		service_ptr get_service()
		{
			auto iter =
				std::find_if(services_.begin(), services_.end(), [](auto srv) { return srv->try_lock(); });

			if (iter == services_.end())
				return nullptr;

			return *iter;
		}

		template <typename F>
		void push_queue(F&& task)
		{
			std::lock_guard lk(mutex_);

			queue_.push([task = std::move(task)] { task(); });
		}

	private:
		std::vector<service_ptr> services_;

		io_service_pool& pools_;

		std::shared_ptr<std::thread> thread_ptr_;

		std::atomic_bool has_stop_;

		std::size_t conn_num_;

		std::deque<std::function<void()>> queue_;

		std::mutex mutex_;
	};
} // namespace aquarius