#pragma once
#include "../common.hpp"

namespace aquarius
{
	namespace detail
	{
		class parse
		{
		public:
			parse() = default;
			virtual ~parse() = default;

		public:
			virtual void parse_bytes(streambuf& ios) = 0;

			virtual void to_bytes(streambuf& ios) = 0;
		};
	}
}