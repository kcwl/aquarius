#pragma once
#include <array>
#include <string_view>

namespace aquarius
{
	namespace detail
	{
		template <char... args>
		struct to_string
		{
			static constexpr std::array<char, sizeof...(args)> value = { args... };

			constexpr operator std::string_view() const
			{
				return std::string_view(value.data(), value.size());
			}
		};

		template <std::size_t N, char... args>
		constexpr auto int_to_string() -> std::string_view
		{
			if constexpr (N < 10)
			{
				return to_string<N + '0', args...>();
			}
			else
			{
				return int_to_string<N / 10, N % 10 + '0', args...>();
			}
		}
	} // namespace detail
} // namespace aquarius