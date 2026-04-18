#pragma once
#include <aquarius/concepts.hpp>

namespace aquarius
{
	template <typename T>
	T add_string(const T& value)
	{
		using type = std::remove_cvref_t<T>;

		if constexpr (string_t<type>)
		{
			return std::string("\"") + value + std::string("\"");
		}
		else
		{
			return value;
		}
	}
} // namespace aquarius