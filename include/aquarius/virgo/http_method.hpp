#pragma once
#include <ostream>
#include <string>

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

		inline std::string from_method_string(http_method m)
		{
			switch (m)
			{
				case http_method::get:
					return "GET";
				case http_method::post:
					return "POST";
				case http_method::unknown:
				default:
					break;
			}

			return "UNKNOWN";
		}

	} // namespace virgo
} // namespace aquarius