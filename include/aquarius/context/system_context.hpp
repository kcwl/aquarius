#pragma once
#include <aquarius/context/context.hpp>
#include <chrono>

using namespace std::chrono_literals;

namespace aquarius
{
	template <std::size_t Number>
	class system_context : public context
	{
		static_assert(Number >= 500 && Number < 1000, "system context number must be 500 ~ 999");

	public:
		constexpr static std::size_t Number = Number;

	public:
		system_context(const std::string& name)
			: context(name, 1s)
		{}
	};
} // namespace aquarius