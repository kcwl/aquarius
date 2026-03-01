#pragma once
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/io_service_pool.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/module/module.hpp>
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
			: mutex_()
			, routers_()
			, pool_(std::thread::hardware_concurrency() / 2)
			, pool_run_thread_ptr_(nullptr)
		{}

		~module_router()
		{
			pool_.stop();

			pool_run_thread_ptr_->join();
		}

	public:
		template <typename Module>
		void regist()
		{
			std::unique_lock lock(mutex_);

			constexpr auto module_name = detail::struct_name<Module>();

			routers_.insert({ std::string(module_name),
							  std::make_shared<Module>(pool_.get_io_service(), std::string(module_name)) });
		}

		void run()
		{
			for (auto& f : routers_)
			{
				if (!f.second)
				{
					continue;
				}

				if (!f.second->config())
				{
					continue;
				}

				if (!f.second->init())
				{
					XLOG_WARNING() << "[" << f.second->name() << "] init error";
					continue;
				}

				co_spawn(f.second->get_executor(), [&]() -> awaitable<void> { co_await f.second->run(); }, detached);
			}

			pool_run_thread_ptr_ = std::make_shared<std::thread>([&] { pool_.run(); });
		}

		template <typename T, typename Task>
		auto schedule(std::shared_ptr<Task> task) -> awaitable<typename Task::return_type>
		{
			std::shared_lock lock(mutex_);

			using return_type = typename Task::return_type;

			constexpr auto module_name = detail::struct_name<T>();

			auto iter = routers_.find(std::string(module_name));
			if (iter == routers_.end())
			{
				XLOG_WARNING() << "[" << module_name << "] module not found";
				co_return return_type{};
			}

			auto temp_module = std::dynamic_pointer_cast<basic_module<T>>(iter->second);
			if (!temp_module)
			{
				XLOG_WARNING() << "[" << module_name << "] task type not match";
				co_return return_type{};
			}

			co_return co_await co_spawn(
				temp_module->get_executor(),
				[&]() -> awaitable<return_type> { co_return co_await temp_module->visit(task); }, use_awaitable);
		}

		void hot_update(const std::string& module_name)
		{
			auto iter = routers_.find(module_name);
			if (iter == routers_.end())
				return;

			iter->second->stop();

			run();
		}

		void timer(std::chrono::milliseconds ms)
		{
			std::shared_lock lock(mutex_);

			for (auto& f : routers_)
			{
				if (!f.second)
					continue;

				f.second->timer(ms);
			}
		}

	private:
		std::shared_mutex mutex_;

		std::map<std::string, std::shared_ptr<module_base>> routers_;

		io_service_pool pool_;

		std::shared_ptr<std::thread> pool_run_thread_ptr_;
	};

	template <typename Module>
	struct auto_module_register
	{
		auto_module_register(const std::string& module_name)
		{
			module_router::get_mutable_instance().regist<Module>(module_name);
		}
	};
} // namespace aquarius

#define AQUARIUS_MODULE_CAT(r, m) r##m

#define AQUARIUS_MODULE_STR(str) #str

#define AQUARIUS_MODULE(__method)                                                                                      \
	[[maybe_unused]] static aquarius::auto_module_register<__method> AQUARIUS_MODULE_CAT(__auto_register_, __method)(  \
		AQUARIUS_MODULE_STR(__method));

#define AQUARIUS_MODULE_NAMESPACE(__namespace, __method)                                                               \
	[[maybe_unused]] static aquarius::auto_module_register<__namespace::__method> AQUARIUS_MODULE_CAT(                 \
		__auto_register_, __method)(AQUARIUS_MODULE_STR(__method));\
