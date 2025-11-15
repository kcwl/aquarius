#pragma once
#include <ostream>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace virgo
	{
		enum class http_version
		{
			http1_0,
			http1_1,
			http2,
			http3,
			unknown
		};

		inline std::ostream& operator<<(std::ostream& os, http_version v)
		{
			os << static_cast<int>(v);

			return os;
		}

		inline std::string_view from_string_version(http_version v)
		{
			switch (v)
			{
			case http_version::http1_0:
				return "HTTP/1.0"sv;
			case http_version::http1_1:
				return "HTTP/1.1"sv;
			case http_version::http2:
				return "HTTP/2"sv;
			case http_version::http3:
				return "HTTP/3"sv;
			default:
				break;
			}

			return "Unknown";
		}

		inline http_version from_version_string(std::string_view s)
		{
			if (s == "HTTP/1.0")
				return http_version::http1_0;
			else if (s == "HTTP/1.1")
				return http_version::http1_1;
			else if (s == "HTTP/2")
				return http_version::http2;
			else if (s == "HTTP/3")
				return http_version::http3;

			return http_version::unknown;
		}
	} // namespace http
} // namespace virgo