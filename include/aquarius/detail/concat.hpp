#pragma once
#include <string_view>
#include <array>

namespace aquarius
{
	namespace detail
	{
		template <const auto&... args>
		struct concat
		{
			constexpr static auto impl() noexcept
			{
				constexpr auto len = (args.size() + ... + 0);
				std::array<char, len + 1> arr{};

				int i = 0;

				auto f = [&i, &arr](const auto& str) mutable
				{
					for (auto s : str)
						arr[i++] = s;

					return arr;
				};

				(f(args), ...);

				arr[len] = '\0';

				return arr;
			}

			static constexpr auto arr = impl();

			static constexpr std::string_view value{ arr.data(), arr.size() - 1 };
		};

		template <const auto&... args>
		constexpr static auto concat_v = concat<args...>::value;
	} // namespace detail
} // namespace aquarius