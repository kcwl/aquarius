#pragma once
#include <string_view>

namespace aquarius
{
	namespace detail
	{
		template <std::size_t N>
		struct string_literal
		{
			constexpr string_literal(const char (&arr)[N])
			{
				std::copy_n(arr, N, value);

				size = N;
			}

			char value[N];

			int size;
		};

		template <string_literal param>
		struct bind_param
		{
			static constexpr std::string_view value = param.value;
		};
	} // namespace detail
} // namespace aquarius