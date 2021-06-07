#pragma once
#include <functional>
#include <tuple>

namespace aquarius
{
	namespace detail
	{
		template<typename _Ty, typename _ = void>
		struct is_container : std::false_type {};

		template<typename _Ty>
		struct is_container<_Ty, std::void_t<typename _Ty::value_type,
			typename _Ty::size_type,
			typename _Ty::allocator_type,
			typename _Ty::iterator,
			typename _Ty::const_iterator,
			decltype(std::declval<_Ty>().size()),
			decltype(std::declval<_Ty>().begin()),
			decltype(std::declval<_Ty>().end()),
			decltype(std::declval<_Ty>().cbegin()),
			decltype(std::declval<_Ty>().cend())>> : std::true_type{};

		template<class _Ty>
		constexpr static bool is_container_v = is_container<std::remove_cvref_t<_Ty>>::value;
	}
}

