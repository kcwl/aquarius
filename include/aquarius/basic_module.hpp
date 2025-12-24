#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>
#include <chrono>
#include <string>

namespace aquarius
{
	class module_base
	{
	public:
		module_base(const std::string& name)
			: name_(name)
		{}

		virtual ~module_base() = default;

	public:
		const std::string& name() const
		{
			return name_;
		}

	public:
		virtual bool config() = 0;

		virtual bool init() = 0;

		virtual auto run() -> awaitable<void> = 0;

		virtual void stop() = 0;

		virtual void timer(std::chrono::milliseconds) = 0;

	private:
		std::string name_;
	};

	template <typename T>
	class basic_module : public module_base
	{
	public:
		using core_type = T;

	public:
		basic_module(const std::string& name)
			: module_base(name)
		{}

	public:
		template <typename Task>
		auto visit(std::shared_ptr<Task> task_ptr) -> awaitable<typename Task::return_type>
		{
			using return_type = typename Task::return_type;
			if (!task_ptr)
				co_return return_type{};

			co_return co_await (*task_ptr)(core());
		}

	protected:
		virtual std::shared_ptr<T> core() = 0;
	};
} // namespace aquarius