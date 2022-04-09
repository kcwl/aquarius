#pragma once
#include <memory>
#include "../detail/empty_value.hpp"

namespace aquarius
{
	namespace msg
	{
		template<typename  T, std::size_t N = 0>
		class basic_fields : public detail::empty_value<typename T, N>
		{
		public:
			static_assert(std::is_trivial_v<T>&& std::is_standard_layout_v<T>, "header field is not pod type!");

			using value_type = T;

			using base_type = detail::empty_value<typename T, N>;
		public:
			basic_fields()
				: base_type(detail::empty_init_t{})
			{

			}

			basic_fields(const T& alloc) noexcept
				: base_type(detail::empty_init, alloc)
			{

			}
		};
	}
}