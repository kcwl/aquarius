#pragma once
#include <ostream>

namespace aquarius
{
	namespace virgo
	{
		enum class http_method
		{
			get,
			post,
			unknown
		};

		inline std::ostream& operator<<(std::ostream& os, http_method m)
		{
			os << static_cast<int>(m);

			return os;
		}
	} // namespace virgo
} // namespace aquarius