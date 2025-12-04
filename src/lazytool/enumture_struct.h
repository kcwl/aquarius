#pragma once
#include "parser.h"
#include "data_field.h"

namespace aquarius
{
	namespace lazytool
	{

		class enum_struct : public data_field, public parser
		{
		public:
			enum_struct();
			~enum_struct() = default;

		public:
			friend std::ostream& operator<<(std::ostream& os, const enum_struct& field)
			{
				os << "enum " << field.name() << std::endl;
				os << "{" << std::endl;

				for (auto& s : field.fields())
				{
					os << "\t" << s.second << "," << std::endl;
				}

				os << "};" << std::endl;

				return os;
			}

		public:
			virtual parse_error visit(std::ifstream& ifs, std::size_t& column, std::size_t& row) override;
		};
	} // namespace lazytool
} // namespace aquarius