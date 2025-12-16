#pragma once
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
		template <typename Task, typename Config, auto ConfigPath>
		void regist(std::string_view module_name)
		{
			std::lock_guard lk(mutex_);

			routers_.insert({ module_name, std::make_shared<_module<Task, Config, ConfigPath>>(module_name) });
		}

		bool run()
		{
			std::lock_guard lk(mutex_);

			for (auto& f : routers_)
			{
				if (!f.second)
					continue;

				if (!f.second->config())
				{
					return false;
				}

				if (!f.second->init())
				{
					XLOG_WARNING() << "[" << f.second->name() << "] init error";
					return false;
				}
			}

			return true;
		}

		template <typename IoContext, typename Task>
		void schedule(IoContext& io, std::string_view module_name, std::shared_ptr<Task> task)
		{
			std::lock_guard lk(mutex_);

			auto iter = routers_.find(module_name);
			if (iter == routers_.end())
			{
				XLOG_WARNING() << "[" << module_name << "] module not found";
				return;
			}

			auto temp_module = std::dynamic_pointer_cast<basic_module<Task>>(iter->second);
			if (!temp_module)
			{
				XLOG_WARNING() << "[" << module_name << "] task type not match";
				return;
			}

			co_spawn(
				io,
				[&] -> awaitable<void>
				{
					auto ec = co_await temp_module->visit(task);
					if (ec)
					{
						XLOG_ERROR() << "[" << temp_module->name() << "] handle error: " << ec.message();
					}
				},
				detached);
		}

		void hot_update(std::string_view module_name)
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

	private:
		std::mutex mutex_;

		std::map<std::string_view, std::shared_ptr<module_base>> routers_;
	};

	template <typename Task, typename Config, auto ConfigPath>
	struct auto_module_register
	{
		auto_module_register(std::string_view module_name)
		{
			module_router::get_mutable_instance().regist<Task, Config, ConfigPath>(module_name);
		}
	};
} // namespace aquarius