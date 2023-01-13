#pragma once
#include <aquarius/detail/type_traits.hpp>
#include <cstddef>
#include <boost/core/empty_value.hpp>

namespace aquarius
{
	namespace detail
	{
		template<typename _Alloc>
		using empty_value = boost::empty_::empty_value<_Alloc>;

		inline static constexpr boost::empty_init_t empty_init = boost::empty_init;
	} // namespace detail
} // namespace aquarius