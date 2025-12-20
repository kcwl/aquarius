#pragma once
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/module/module.hpp>
#include <aquarius/singleton.hpp>
#include <map>
#include <mutex>

namespace aquarius
{
	class module_router : public singleton<module_router>
	{
	public:
		module_router() = default;

		~module_router() = default;

	public:
		template <typename Core, typename Config, auto ConfigPath>
		void regist(const std::string& module_name)
		{
			std::lock_guard lk(mutex_);

			routers_.insert({ module_name, std::make_shared<_module<Core, Config, ConfigPath>>(module_name) });
		}

		auto run() -> awaitable<void>
		{
			std::lock_guard lk(mutex_);

			for (auto& f : routers_)
			{
				if (!f.second)
					continue;

				if (!f.second->config())
				{
					co_return;
				}

				if (!f.second->init())
				{
					XLOG_WARNING() << "[" << f.second->name() << "] init error";
					co_return;
				}

				co_await f.second->run();
			}
		}

		template <typename Core, typename Task>
		auto schedule(const std::string& module_name, std::shared_ptr<Task> task) -> awaitable<typename Task::return_type>
		{
			using return_type = typename Task::return_type;

			std::lock_guard lk(mutex_);

			auto iter = routers_.find(module_name);
			if (iter == routers_.end())
			{
				XLOG_WARNING() << "[" << module_name << "] module not found";
				co_return return_type{};
			}

			auto temp_module = std::dynamic_pointer_cast<basic_module<Core>>(iter->second);
			if (!temp_module)
			{
				XLOG_WARNING() << "[" << module_name << "] task type not match";
				co_return return_type{};
			}

			// auto ec = co_await temp_module->visit(task);
			co_return co_await temp_module->visit(task);
			// if (ec)
			//{
			//	XLOG_ERROR() << "[" << temp_module->name() << "] handle error: " << ec.message();
			// }

			// co_return temp_module->result();
		}

		void hot_update(const std::string& module_name)
		{
			std::lock_guard lk(mutex_);

			auto iter = routers_.find(module_name);
			if (iter == routers_.end())
				return;

			iter->second->stop();

			if (!iter->second->init())
			{
				XLOG_WARNING() << "[" << iter->second->name() << "] init error";
				return;
			}
		}

		void timer(std::chrono::milliseconds ms)
		{
			std::lock_guard lk(mutex_);

			for (auto& f : routers_)
			{
				if (!f.second)
					continue;

				f.second->timer(ms);
			}
		}

	private:
		std::mutex mutex_;

		std::map<std::string, std::shared_ptr<module_base>> routers_;
	};

	template <typename Core, typename Config, detail::string_literal ConfigPath>
	struct auto_module_register
	{
		auto_module_register(const std::string& module_name)
		{
			module_router::get_mutable_instance().regist<Core, Config, ConfigPath>(module_name);
		}
	};
} // namespace aquarius

#define AQUARIUS_MODULE_CAT(r, m) r##m

#define AQUARIUS_MODULE_STR(str) #str

#define AQUARIUS_MODULE(__method)                                                                                      \
	[[maybe_unused]] static aquarius::auto_module_register<typename __method::core_type, __method::config_type,        \
														   __method::config_path>                                      \
		AQUARIUS_MODULE_CAT(__auto_register_, __method)(AQUARIUS_MODULE_STR(__method));\
