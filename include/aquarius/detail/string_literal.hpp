#pragma once
#include <string_view>
#include <cstddef>

namespace aquarius
{
	namespace detail
	{
		template <std::size_t N>
		struct string_literal
		{
			constexpr string_literal(const char (&arr)[N])
			{
				for (std::size_t i = 0; i < N; ++i)
					value[i] = arr[i];
			}

			char value[N];

			static constexpr std::size_t size = N;
		};

		template <string_literal param>
		struct bind_param
		{
			static constexpr std::string_view value{param.value, param.size - 1};
		};
	} // namespace detail
} // namespace aquarius