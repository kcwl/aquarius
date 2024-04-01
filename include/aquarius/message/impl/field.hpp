#pragma once
#include <aquarius/core/empty_value.hpp>

namespace aquarius
{
	namespace impl
	{
		template <typename _Alloc, typename _Traits>
		class basic_fields : private empty_value<_Alloc>
		{
		public:
			using allocate_type = _Alloc;

			using value_type = typename _Alloc::value_type;

		public:
			explicit basic_fields() noexcept
				: empty_value<_Alloc>(empty_init, allocate_type{})
			{}

			virtual ~basic_fields() = default;

		protected:
			void alloc(value_type*& ptr)
			{
				auto mem = _Traits::allocate(this->get(), 1);

				ptr = ::new (mem) value_type{};
			}

			void dealloc(value_type*& ptr)
			{
				_Traits::deallocate(this->get(), ptr, 1);

				ptr = nullptr;
			}
		};
	} // namespace impl
} // namespace aquarius