#pragma once
#include <aquarius/basic_module.hpp>
#include <aquarius/io_service_pool.hpp>
#include <aquarius/detail/struct_name.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/singleton.hpp>
#include <map>
#include <mutex>
#include <shared_mutex>

namespace aquarius
{
	class module_router : public singleton<module_router>
	{
	public:
		module_router()
			: routers_()
		{}

		~module_router() = default;

	public:
		template <typename Module>
		bool regist()
		{
			auto module_ptr = std::make_shared<Module>();

			auto iter = routers_.find(module_ptr->name());

			if (iter != routers_.end())
			{
				return false;
			}

			routers_.insert({ module_ptr->name(), module_ptr });

			return true;
		}

		void run(io_service_pool& pool)
		{
			for (auto& f : routers_)
			{
				run_one(pool.get_io_service(), f.second);
			}
		}

		template <typename T, typename R, typename Func>
		auto async_schedule(Func&& f) -> asio::awaitable<R>
		{
			constexpr auto module_name = detail::struct_name<T>();

			auto iter = routers_.find(std::string(module_name));
			if (iter == routers_.end())
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

			auto iter = routers_.find(std::string(module_name));
			if (iter == routers_.end())
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
			for (auto& f : routers_)
			{
				if (!f.second)
					continue;

				f.second->timer(ms);
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
		std::unordered_map<std::string, std::shared_ptr<module_base>> routers_;
	};
} // namespace aquarius
