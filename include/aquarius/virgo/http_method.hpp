#pragma once
#include <virgo/detail/config.hpp>
#include <ostream>

namespace aquarius
{
	namespace virgo
	{
		enum class method
		{
			get,
			post,
			unknown
		};

		inline std::ostream& operator<<(std::ostream& os, method m)
		{
			os << static_cast<int>(m);

			return os;
		}
	} // namespace virgo
} // namespace aquarius