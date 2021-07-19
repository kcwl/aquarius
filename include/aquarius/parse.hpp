#pragma once
#include "detail/streambuf.hpp"

namespace aquarius
{
	class parse
	{
	public:
		parse() = default;
		virtual ~parse() = default;

	public:
		virtual std::size_t parse_bytes(detail::streambuf& ios) = 0;

		virtual std::size_t to_bytes(detail::streambuf& ios) = 0;
	};
}
