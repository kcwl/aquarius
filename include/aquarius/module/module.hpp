#pragma once
#include <aquarius/basic_module.hpp>
#include <aquarius/logger.hpp>

namespace aquarius
{
	template <typename Task, typename Config, auto ConfigPath>
	class _module : public basic_module<Task>
	{
	public:
		_module(const std::string& name)
			: basic_module<Task>(name)
		{}

		virtual ~_module() = default;

	public:
		virtual void config() override
		{
			error_code ec{};
			config_ = ini::parse<Config>(std::ifstream{ ConfigPath }, ec);

			if (!ec)
				return;

			XLOG_ERROR() << "parse config error: " << ec.message();

			throw ec;
		}

		Config config() const
		{
			return config_;
		}

		virtual bool init() override
		{
			return true;
		}

		virtual void stop() override
		{
			return;
		}

		virtual void timer(std::chrono::milliseconds) override
		{
			return;
		}

	private:
		Config config_;
	};
} // namespace aquarius

#define AQUARIUS_MODULE_CONFIG()                                                                                       \
public:                                                                                                                \
	virtual bool config() override

#define AQUARIUS_MODULE_INIT()                                                                                         \
public:                                                                                                                \
	virtual bool init() override

#define AQUARIUS_MODULE_STOP()                                                                                         \
public:                                                                                                                \
	virtual void stop() override

#define AQUARIUS_MODULE_TIMER(ms)                                                                                      \
public:                                                                                                                \
	virtual void timer(std::chrono::milliseconds ms) override

#define AQUARIUS_MODULE_VISIT()                                                                                        \
public:                                                                                                                \
	virtual auto handle() -> awaitable<error_code> override

#define AQUARIUS_MODULE(__method, __task, __config, __config_path)                                                     \
	class __method;                                                                                                    \
	[[maybe_unused]] static aquarius::auto_module_register<__task, __config, __config_path>                            \
		__auto_register_##__method(##__method);                                                                        \
	class __method : public module<__task, __config, __config_path>
