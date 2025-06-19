#pragma once
#include <aquarius/use_future.hpp>
#include <chrono>
#include <string>

using namespace std::chrono_literals;

namespace aquarius
{
	class context_base
	{
	public:
		context_base(const std::string& name, std::chrono::steady_clock::duration timeout)
			: name_(name)
			, timeout_(timeout)
		{}

	public:
		std::string ctx_name() const
		{
			return name_;
		}

		auto timeout() const
		{
			return timeout_;
		}

		template <typename Executor, typename Func>
		auto invoke(const Executor& ex, Func&& f)
		{
			auto future = co_spawn(ex, [func = std::move(f)] -> awaitable<void> { func(); co_return; }, use_future);

			auto status = future.wait_for(timeout_);

			return status;
		}

	private:
		std::string name_;

		std::chrono::steady_clock::duration timeout_;
	};
} // namespace aquarius