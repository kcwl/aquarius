#pragma once
#include <type_traits>

namespace aquarius
{
	template <typename T>
	struct is_client : std::false_type
	{};
} // namespace aquarius