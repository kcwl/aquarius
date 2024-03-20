#pragma once
#include <aquarius/system/empty_value.hpp>

namespace aquarius
{
	namespace impl
	{
		template <typename _Alloc, typename _Traits>
		class basic_fields : private system::empty_value<_Alloc>
		{
		public:
			using allocate_type = _Alloc;

			using value_type = typename _Alloc::value_type;

		public:
			basic_fields() = default;

			basic_fields(const allocate_type& alloc) noexcept
				: system::empty_value<_Alloc>(system::empty_init, alloc)
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