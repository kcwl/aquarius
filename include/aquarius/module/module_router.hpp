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
		using executor_t = asio::any_io_executor;

	public:
		module_router()
			: routers_by_name_()
			, routers_by_priority_()
		{}

		~module_router() = default;

	public:
		template <typename Module>
		bool put(int priority = 0)
		{
			auto module_ptr = std::make_shared<Module>();

			std::string name(detail::struct_name<Module>());

			auto iter = routers_by_name_.find(name);

			if (iter != routers_by_name_.end())
			{
				return false;
			}

			routers_by_name_.insert({ name, module_ptr });
			routers_by_priority_[priority].push_back(module_ptr);

			return true;
		}

		auto run() -> asio::awaitable<void>
		{
			for (auto& pr : routers_by_priority_)
			{
				for (auto& module_ptr : pr.second)
				{
					if (module_ptr.expired())
					{
						continue;
					}

					co_await run_one(module_ptr.lock());
				}
			}
		}

		template <typename T, typename R, typename Func>
		auto async_schedule(Func&& f) -> asio::awaitable<R>
		{
			auto module_ptr = get_module<T>();
			if (!module_ptr)
			{
				co_return R{};
			}

			co_return co_await module_ptr->template async_visit<R>(std::forward<Func>(f));
		}

		template <typename T, typename R, typename Func>
		auto schedule(Func&& f) -> R
		{
			auto module_ptr = get_module<T>();
			if (!module_ptr)
			{
				return R{};
			}

			return module_ptr->template visit<R>(std::forward<Func>(f));
		}

		auto timer(std::chrono::milliseconds ms) -> asio::awaitable<void>
		{
			for (auto& pr : routers_by_priority_)
			{
				for (auto& module_ptr : pr.second)
				{
					if (module_ptr.expired())
					{
						continue;
					}

					co_await module_ptr.lock()->timer(ms);
				}
			}
		}

	private:
		auto run_one(std::shared_ptr<module_base> module_ptr) -> asio::awaitable<bool>
		{
			if (!module_ptr)
			{
				co_return false;
			}

			if (!module_ptr->init())
			{
				XLOG_WARNING() << "[" << module_ptr->name() << "] init error";
				co_return false;
			}

			co_return co_await module_ptr->run();
		}

		template <typename T>
		std::shared_ptr<basic_module<T>> get_module()
		{
			constexpr auto module_name = detail::struct_name<T>();

			auto iter = routers_by_name_.find(std::string(module_name));
			if (iter == routers_by_name_.end())
			{
				XLOG_WARNING() << "[" << module_name << "] module not found";
				return nullptr;
			}

			auto module_ptr = std::dynamic_pointer_cast<basic_module<T>>(iter->second);
			if (!module_ptr)
			{
				XLOG_WARNING() << "[" << module_name << "] task type not match";
				return nullptr;
			}

			return module_ptr;
		}

	private:
		std::unordered_map<std::string, std::shared_ptr<module_base>> routers_by_name_;

		std::map<int, std::vector<std::weak_ptr<module_base>>, std::greater<int>> routers_by_priority_;
	};
} // namespace aquarius
