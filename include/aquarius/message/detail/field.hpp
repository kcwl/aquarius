#pragma once
#include <memory>
#include "../../detail/empty_value.hpp"

namespace aquarius
{
	namespace msg
	{
		template<typename  T, std::size_t N = 0>
		class basic_fields : private aquarius::detail::empty_value<typename T, N>
		{
		public:
			using value_type = T;

			using base_type = aquarius::detail::empty_value<typename T, N>;
		public:
			basic_fields()
				: base_type(aquarius::detail::empty_init_t{})
			{

			}

			basic_fields(const T& alloc) noexcept
				: base_type(aquarius::detail::empty_init, alloc)
			{

			}

			auto& header_value() noexcept
			{
				return base_type::get();
			}

			auto& header_value() const noexcept
			{
				return base_type::get();
			}

		};
	}
}