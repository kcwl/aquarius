#pragma once
#include "parser.h"

namespace aquarius
{
	namespace lazytool
	{
		class domin_parse : public parser
		{
		public:
			domin_parse();
			virtual ~domin_parse() = default;

		public:
			std::ostream& operator<<(std::ostream& os) const;

		public:
			virtual parse_error visit(std::ifstream& ifs, std::size_t& column, std::size_t& row) override;

		private:
			std::string read_domain(std::ifstream& ifs, std::size_t& column, std::size_t& row);
		};

		std::ostream& operator<<(std::ostream& os, const domin_parse& dp);
	} // namespace lazytool
} // namespace aquarius