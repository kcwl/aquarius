#pragma once
#include <ostream>

namespace aquarius
{
	namespace virgo
	{
		enum class http_version
		{
			http1_0,
			http1_1,
			http2,
			http3
		};

		inline std::ostream& operator<<(std::ostream& os, http_version v)
		{
			os << static_cast<int>(v);

			return os;
		}

		inline std::string version_to_string(http_version v)
		{
			switch (v)
			{
			case http_version::http1_0:
				return "HTTP/1.0";
			case http_version::http1_1:
				return "HTTP/1.1";
			case http_version::http2:
				return "HTTP/2";
			case http_version::http3:
				return "HTTP/3";
			}

			return {};
		}
	} // namespace http
} // namespace virgo