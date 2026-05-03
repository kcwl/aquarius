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
	class module_router : public singleton<module_router>
	{
	public:
		module_router()
			: routers_by_name_()
			, routers_by_priority_()
		{}

		~module_router() = default;

	public:
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
			// run modules by priority (higher priority first)
			for (auto& pr : routers_by_priority_)
			{
				for (auto& module_ptr : pr.second)
				{
					run_one(pool.get_io_service(), module_ptr);
				}
			}
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

			co_return co_await temp_module->template async_visit<R>(std::forward<Func>(f));
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
					if (!module_ptr)
						continue;

					// call timer coroutine (return awaitable) and ignore it to mimic previous behavior
					module_ptr->timer(ms);
				}
			}
		}

	private:
		void run_one(asio::io_context& io, std::shared_ptr<module_base> module_ptr)
		{
			if (!module_ptr)
			{
				return;
			}

			if (!module_ptr->init())
			{
				XLOG_WARNING() << "[" << module_ptr->name() << "] init error";
				return;
			}

			co_spawn(
				io, [&, module_ptr]() -> asio::awaitable<bool> { co_return co_await module_ptr->run(); },
				asio::detached);
		}

	private:
		std::unordered_map<std::string, std::shared_ptr<module_base>> routers_by_name_;

		std::map<int, std::vector<std::shared_ptr<module_base>>, std::greater<int>> routers_by_priority_;
	};
} // namespace aquarius
