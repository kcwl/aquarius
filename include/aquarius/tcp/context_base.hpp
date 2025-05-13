#pragma once
#include <chrono>
#include <string>

using namespace std::chrono_literals;

namespace aquarius
{
	namespace tcp
	{
		class context_base
		{
		public:
			explicit context_base(const std::string& name, const std::chrono::milliseconds& timeout)
				: ctx_name_(name)
				, timeout_(timeout)
			{}

		protected:
			std::string ctx_name_;

			std::chrono::milliseconds timeout_;
		};
	} // namespace tcp
} // namespace aquarius