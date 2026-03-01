#pragma once
#include <aquarius/detail/concat.hpp>
#include <string>
#include <tuple>

namespace aquarius
{
	namespace tbl
	{
		template <std::size_t I, typename T>
		constexpr auto struct_element_value(T&& value)
		{
			using tp = std::decay_t<T>;
			return value.*std::get<I>(tp::member());
		}

		template <std::size_t I, typename T>
		constexpr auto struct_element_name()
		{
			using tp = std::decay_t<T>;
			return std::get<I>(tp::member_name());
		}

		template <typename T>
		struct remove_class_pointer;

		template <typename T, typename R>
		struct remove_class_pointer<R(T::*)>
		{
			using type = R;
		};

		template <std::size_t I, typename T>
		using struct_element_t = remove_class_pointer<std::tuple_element_t<I, decltype(std::decay_t<T>::member())>>::type;

		template <typename T>
		constexpr auto struct_size()
		{
			using tp = std::decay_t<T>;
			return std::tuple_size_v<decltype(tp::member())>;
		}

		

		template <typename T, typename Tp>
		constexpr bool has_attribute()
		{
			using tp = std::decay_t<Tp>;

			constexpr auto size = std::tuple_size_v<typename tp::type>;

			auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
			{ return std::disjunction_v<std::is_same<T, std::tuple_element_t<I, typename tp::type>>...>; };

			return f(std::make_index_sequence<size>{});
		}

		template <typename Attribute>
		std::string collect_attribute()
		{
			using type = typename Attribute::type;

			constexpr auto size = std::tuple_size_v<type>;

			constexpr auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
			{ return (concat_v<std::tuple_element_t<I, type>::value...>); };

			constexpr auto str = f(std::make_index_sequence<size>{});

			return std::string(str);
		}
	} // namespace tbl
} // namespace aquarius