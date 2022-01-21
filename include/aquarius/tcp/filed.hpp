#pragma once
#include <memory>
#include "../detail/empty_value.hpp"

namespace aquarius
{
	namespace tcp
	{
		template<typename  Allocator, std::size_t N = 0>
		class basic_fileds : private detail::empty_value<Allocator, N>
		{
		public:
			using value_t = Allocator;

			using base_type = detail::empty_value<Allocator, N>;
		public:
			basic_fileds()
				: base_type(detail::empty_init_t{})
			{

			}

			basic_fileds(const Allocator& alloc) noexcept
				: base_type(detail::empty_init, alloc)
			{

			}

		public:
			Allocator& get()
			{
				return base_type::get();
			}

			const Allocator& get() const
			{
				return base_type::get();
			}
		};
	}
}