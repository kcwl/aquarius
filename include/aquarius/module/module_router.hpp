#pragma once
#include <aquarius/basic_module.hpp>
#include <aquarius/detail/struct_name.hpp>
#include <aquarius/io_service_pool.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/singleton.hpp>
#include <functional>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

namespace aquarius
{
	template <typename Executor = asio::any_io_executor>
	class module_router : public singleton<module_router<Executor>>
	{
	public:
		module_router()
			: routers_by_name_()
			, routers_by_priority_()
		{}

		~module_router() = default;

	public:
		const Executor& get_executor()
		{
			return executor_;
		}

		template <typename Module>
		bool regist(int priority = 0)
		{
			auto module_ptr = std::make_shared<Module>();

			auto name = module_ptr->name();

			auto iter = routers_by_name_.find(name);

			if (iter != routers_by_name_.end())
			{
				return false;
			}

			routers_by_name_.insert({ name, module_ptr });
			routers_by_priority_[priority].push_back(module_ptr);

			return true;
		}

		void run(io_service_pool& pool)
		{
			auto& io = pool.get_io_service();

			attach_thread(io.get_executor());

			asio::co_spawn(
				get_executor(),
				[&]() -> asio::awaitable<void>
				{
					for (auto& pr : routers_by_priority_)
					{
						for (auto& module_ptr : pr.second)
						{
							if (module_ptr.expired())
							{
								continue;
							}

							co_await run_one(pool, module_ptr.lock());
						}
					}
				},
				asio::detached);
		}

		template <typename T, typename R, typename Func>
		auto async_schedule(Func&& f) -> asio::awaitable<R>
		{
			constexpr auto module_name = detail::struct_name<T>();

			auto iter = routers_by_name_.find(std::string(module_name));
			if (iter == routers_by_name_.end())
			{
				XLOG_WARNING() << "[" << module_name << "] module not found";
				co_return R{};
			}

			auto temp_module = std::dynamic_pointer_cast<basic_module<T>>(iter->second);
			if (!temp_module)
			{
				XLOG_WARNING() << "[" << module_name << "] task type not match";
				co_return R{};
			}

			co_return co_await asio::co_spawn(
				temp_module->get_executor(), [&, temp_module]() -> asio::awaitable<R>
				{ co_return co_await temp_module->template async_visit<R>(std::forward<Func>(f)); },
				asio::use_awaitable);
		}

		template <typename T, typename R, typename Func>
		auto schedule(Func&& f) -> R
		{
			constexpr auto module_name = detail::struct_name<T>();

			auto iter = routers_by_name_.find(std::string(module_name));
			if (iter == routers_by_name_.end())
			{
				XLOG_WARNING() << "[" << module_name << "] module not found";
				return R{};
			}

			auto temp_module = std::dynamic_pointer_cast<basic_module<T>>(iter->second);
			if (!temp_module)
			{
				XLOG_WARNING() << "[" << module_name << "] task type not match";
				return R{};
			}

			return temp_module->template visit<R>(std::forward<Func>(f));
		}

		void timer(std::chrono::milliseconds ms)
		{
			for (auto& pr : routers_by_priority_)
			{
				for (auto& module_ptr : pr.second)
				{
					if (module_ptr.expired())
					{
						continue;
					}

					auto executor_ptr =
						std::dynamic_pointer_cast<basic_executor_module<asio::any_io_executor>>(module_ptr.lock());

					if (!executor_ptr)
					{
						continue;
					}

					asio::co_spawn(
						executor_ptr->get_executor(), [&, executor_ptr]() -> asio::awaitable<void>
						{ co_await executor_ptr->timer(ms); }, asio::detached);
				}
			}
		}

	private:
		auto run_one(io_service_pool& pool, std::shared_ptr<module_base> module_ptr) -> asio::awaitable<bool>
		{
			if (!module_ptr)
			{
				co_return false;
			}

			auto ptr = std::dynamic_pointer_cast<basic_executor_module<asio::any_io_executor>>(module_ptr);

			if (!ptr)
			{
				co_return false;
			}

			ptr->attach_thread(pool.get_io_service().get_executor());

			if (!ptr->init())
			{
				XLOG_WARNING() << "[" << ptr->name() << "] init error";
				co_return false;
			}

			co_return co_await asio::co_spawn(
				ptr->get_executor(), [&, ptr]() -> asio::awaitable<bool> { co_return co_await ptr->run(pool); },
				asio::use_awaitable);
		}

		void attach_thread(const asio::any_io_executor& executor)
		{
			executor_ = executor;
		}

	private:
		std::unordered_map<std::string, std::shared_ptr<module_base>> routers_by_name_;

		std::map<int, std::vector<std::weak_ptr<module_base>>, std::greater<int>> routers_by_priority_;

		asio::any_io_executor executor_;
	};
} // namespace aquarius
