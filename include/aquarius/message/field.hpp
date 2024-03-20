#pragma once
#include <aquarius/message/impl/field.hpp>
#include <memory>

namespace aquarius
{
	template <typename _Ty>
	using fields = impl::basic_fields<std::allocator<_Ty>, std::allocator_traits<std::allocator<_Ty>>>;
} // namespace aquarius