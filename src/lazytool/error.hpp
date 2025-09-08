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
            syntax,
            routers,
            domains,
            type
        };
	} // namespace virgo
} // namespace aquarius