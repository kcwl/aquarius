#pragma once
#include <fstream>

namespace aquarius
{
	namespace lazytool
	{
		class parse_base
		{
		public:
			enum parse_error
			{
				success,
				ending,
				eof,
				keyword,
				syntax,
				routers,
				routers_mode,
				routers_value,
				domains,
				type
			};

		public:
			parse_base() = default;
			virtual ~parse_base() = default;

		public:
			virtual parse_error visit(std::ifstream& ifs, std::size_t& column, std::size_t& row) = 0;
		};
	} // namespace lazytool
} // namespace aquarius
