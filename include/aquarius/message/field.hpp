#pragma once
#include <memory>
#include "../core/empty_value.hpp"

namespace aquarius
{
	namespace message
	{
		namespace detail
		{
			template<typename _Alloc>
			class basic_fields : private core::empty_value<_Alloc>
			{
			public:
				using allocate_type = _Alloc;

				using value_type = typename _Alloc::value_type;
			public:
				basic_fields() = default;

				basic_fields(const allocate_type& alloc) noexcept
					: core::empty_value<_Alloc>(core::empty_init, alloc)
				{

				}

				virtual ~basic_fields() = default;

			protected:
				void alloc(value_type*& ptr)
				{
					auto p = this->get().allocate(sizeof(value_type));

					ptr = ::new(p)value_type();
				}

				void dealloc(value_type*& ptr)
				{
					this->get().deallocate(ptr, sizeof(value_type));

					ptr = nullptr;
				}
			};
		}
		
		template<typename _Ty>
		using fields = detail::basic_fields<std::allocator<_Ty>>;
	}
}