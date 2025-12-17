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

		virtual void stop() = 0;

		virtual void timer(std::chrono::milliseconds) = 0;

	private:
		std::string name_;
	};

	template <typename Task>
	class basic_module : public module_base
	{
		using return_type = typename Task::return_type;

	public:
		basic_module(const std::string& name)
			: module_base(name)
			, task_ptr_(nullptr)
		{}

	public:
		virtual auto visit(std::shared_ptr<Task> task_ptr) -> awaitable<error_code>
		{
			task_ptr_ = task_ptr;

			co_return co_await handle();
		}

		std::shared_ptr<Task> get_task()
		{
			return task_ptr_;
		}

		return_type result() const
		{
			return result_;
		}

	protected:
		virtual auto handle() -> awaitable<error_code> = 0;

	private:
		std::shared_ptr<Task> task_ptr_;

		return_type result_;
	};
} // namespace aquarius