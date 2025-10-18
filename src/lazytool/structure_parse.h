#pragma once
#include "parser.h"

namespace aquarius
{
	namespace lazytool
	{
		class structure_parse : public parser
		{
		public:
			structure_parse();
			virtual ~structure_parse() = default;

		public:
			virtual parse_error visit(std::ifstream& ifs, std::size_t& column, std::size_t& row) override;
		};
	} // namespace lazytool
} // namespace aquarius