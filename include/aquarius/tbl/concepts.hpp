#pragma once

namespace aquarius
{
	template <typename T>
	struct remove_member_pointer
	{
		using type = T;
	};

	template <typename R, typename T>
	struct remove_member_pointer<T(R::*)>
	{
		using type = T;
	};

	template <typename T>
	struct member_pointer_struct
	{
		using type = T;
	};

	template <typename R, typename T>
	struct member_pointer_struct<T(R::*)>
	{
		using type = R;
	};
} // namespace aquarius