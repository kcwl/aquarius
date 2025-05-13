#pragma once
#include <concepts>

namespace aquarius
{
	template <typename T>
	struct is_start : std::false_type
	{};

	template <typename T>
	concept is_startable = is_start<T>::value;

	template <typename T>
	struct is_connector : std::false_type
	{};

	template <typename T>
	concept is_connectable = is_connector<T>::value;
} // namespace aquarius