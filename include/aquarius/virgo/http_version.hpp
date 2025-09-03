#pragma once
#include <ostream>
#include <virgo/detail/config.hpp>

namespace aquarius
{
	namespace virgo
	{
		enum class version
		{
			http1_0,
			http1_1,
			http2,
			http3
		};

		inline std::ostream& operator<<(std::ostream& os, version v)
		{
			os << static_cast<int>(v);

			return os;
		}

		inline std::string version_to_string(version v)
		{
			switch (v)
			{
			case version::http1_0:
				return "HTTP/1.0";
			case version::http1_1:
				return "HTTP/1.1";
			case version::http2:
				return "HTTP/2";
			case version::http3:
				return "HTTP/3";
			}

			return {};
		}
	} // namespace http
} // namespace virgo