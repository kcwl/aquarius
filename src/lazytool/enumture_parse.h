#pragma once
#include "parser.h"

namespace aquarius
{
	namespace lazytool
	{
		class enumture_parse : public parser
		{
		public:
			enumture_parse();
			virtual ~enumture_parse() = default;

		public:
			virtual parse_error visit(std::ifstream& ifs, std::size_t& column, std::size_t& row) override;
		};
	} // namespace lazytool
} // namespace aquarius