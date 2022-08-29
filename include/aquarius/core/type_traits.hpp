#pragma once
#include <type_traits>

namespace aquarius
{
	namespace  core
	{
		template<typename T>
		struct use_empty_value_base
		{
			static constexpr bool value = std::is_empty_v<T> && !std::is_final_v<T>;
		};

		template<typename T>
		static constexpr bool use_empty_value_base_v = use_empty_value_base<T>::value;

		template<typename T>
		struct is_pod : std::false_type {};

		template<typename T>
			requires(std::is_standard_layout_v<T>&& std::is_trivial_v<T>)
		struct is_pod<T> : std::true_type {};

		template<typename T>
		constexpr static bool is_pod_v = is_pod<std::remove_cvref_t<T>>::value;

		template<typename T, typename _ = std::void_t<>>
		struct is_container : std::false_type {};

		template<typename T>
		struct is_container<T, std::void_t<typename T::value_type,
			typename T::size_type,
			typename T::allocator_type,
			typename T::iterator,
			typename T::const_iterator,
			decltype(std::declval<T>().size()),
			decltype(std::declval<T>().begin()),
			decltype(std::declval<T>().end()),
			decltype(std::declval<T>().cbegin()),
			decltype(std::declval<T>().cend())>> : std::true_type{};

		template<typename T>
		constexpr static bool is_container_v = is_container<T>::value;
	}
}
