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

	template <typename T>
	concept iterator_t = requires() {
		std::begin(std::declval<T>());
		std::end(std::declval<T>());
	};

	template <typename T>
	concept push_t = requires() {
		std::declval<T>().begin();
		std::declval<T>().end();
		std::back_inserter(std::declval<std::add_lvalue_reference_t<T>>());
	};

} // namespace aquarius