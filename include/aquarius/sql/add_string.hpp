#pragma once
#include <string>

namespace aquarius
{
	namespace sql
	{
		template <typename T>
		inline T add_string_format(T value)
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				return T().append("\"").append(value).append("\"");
			}
			else
			{
				return value;
			}
		}
	} // namespace sql
} // namespace aquarius