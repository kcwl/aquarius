#pragma once
#include <aquarius/detail/empty_value.hpp>
#include <memory>

namespace aquarius
{
	namespace impl
	{
		template <typename _Alloc>
		class basic_fields : private detail::empty_value<_Alloc>
		{
		public:
			using allocate_type = _Alloc;

			using value_type = typename _Alloc::value_type;

		public:
			basic_fields() = default;

			basic_fields(const allocate_type& alloc) noexcept
				: detail::empty_value<_Alloc>(detail::empty_init, alloc)
			{}

			virtual ~basic_fields() = default;

		protected:
			void alloc(value_type*& ptr)
			{
				auto p = this->get().allocate(sizeof(value_type));

				ptr = ::new (p) value_type();
			}

			void dealloc(value_type*& ptr)
			{
				this->get().deallocate(ptr, sizeof(value_type));

				ptr = nullptr;
			}
		};
	} // namespace impl

	template <typename _Ty>
	using fields = impl::basic_fields<std::allocator<_Ty>>;
} // namespace aquarius