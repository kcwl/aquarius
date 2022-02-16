#pragma once
#include <memory>
#include "../detail/empty_value.hpp"

namespace aquarius
{
	namespace tcp
	{
		template<typename  Allocator, std::size_t N = 0>
		class basic_fields : private detail::empty_value<Allocator, N>
		{
		public:
			using value_t = typename Allocator::value_type;

			using base_type = detail::empty_value<Allocator, N>;

			using alloc_t = Allocator;
		public:
			basic_fields()
				: base_type(detail::empty_init_t{})
				, field_()
			{

			}

			basic_fields(const alloc_t& alloc) noexcept
				: base_type(detail::empty_init, alloc)
				, field_(alloc)
			{

			}

		public:
			alloc_t& get_alloc()
			{
				return base_type::get();
			}

			const alloc_t& get_alloc() const
			{
				return base_type::get();
			}

			value_t& get()
			{
				return field_;
			}

			const value_t& get() const
			{
				return field_;
			}

			void copy(const value_t& v)
			{
				field_ = v;
			}

		private:
			value_t field_;
		};
	}
}