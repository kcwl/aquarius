#pragma once
#include <concepts>
#include <tuple>

namespace aquarius
{
	namespace detail
	{
		template <std::size_t total, typename T, typename Tp, bool Equal>
		struct find_index;

		template <std::size_t total, typename T, typename Tp>
		struct find_index<total, T, Tp, true>
		{
			constexpr static std::size_t value = total;
		};

		template <std::size_t total, typename T, typename Tp, bool Equal>
		struct find_index : find_index<total - 1, T, Tp, std::same_as<std::tuple_element_t<total - 1, Tp>, T>>
		{};

		template <typename T, typename Tp>
		auto visit(Tp&& tp)
		{
			constexpr auto size = std::tuple_size_v<Tp>;

			constexpr auto index =
				find_index<size, T, Tp, std::same_as<std::tuple_element_t<size - 1, Tp>, T>>::value;

			return std::get<index>(std::forward<Tp>(tp));
		}
	} // namespace detail
} // namespace aquarius