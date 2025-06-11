#pragma once
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

	private:
		std::string name_;

		std::chrono::steady_clock::duration timeout_;
	};
} // namespace aquarius