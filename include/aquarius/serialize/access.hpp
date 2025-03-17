#pragma once
#include <aquarius/core/concepts.hpp>

namespace aquarius
{
	class access
	{
	public:
		template <typename _Archive, typename _Ty>
		static void serialize(_Archive& ar, _Ty&& t)
		{
			std::forward<_Ty>(t).serialize(ar);
		}

		template <typename _Ty, typename _U>
		static _Ty& cast_reference(_U& u)
		{
			return static_cast<_Ty&>(u);
		}
	};

	template <typename _Base, typename _Derived>
	std::conditional_t<std::is_const_v<_Derived>, const _Base, _Base>& base_object(_Derived& d)
	{
		return access::cast_reference<_Base, _Derived>(d);
	}
} // namespace aquarius