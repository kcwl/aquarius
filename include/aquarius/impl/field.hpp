#pragma once
#include <aquarius/detail/empty_value.hpp>
#include <memory>

namespace aquarius
{
	namespace impl
	{
		template <typename _Alloc, typename _Traits>
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

	template <typename _Ty>
	using fields = impl::basic_fields<std::allocator<_Ty>, std::allocator_traits<std::allocator<_Ty>>>;
} // namespace aquarius