#pragma once

namespace aquarius
{
	namespace virgo
	{
		enum parse_error
		{
			success,
			ending,
			eof,
			keyword,
			syntax
		};
	} // namespace virgo
} // namespace aquarius