#pragma once
#include <aquarius/context/context.hpp>
#include <chrono>

using namespace std::chrono_literals;

namespace aquarius
{
	template <std::size_t Number>
	class system_context : public context
	{
		static_assert(Number >= 0 && Number < 1000, "system context number must be 0 ~ 999");

	public:
		system_context(const std::string& name)
			: context(name, 1s)
		{}
	};
} // namespace aquarius