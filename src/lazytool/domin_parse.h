#pragma once
#include "parser.h"
#include "data_field.h"

namespace aquarius
{
	namespace lazytool
	{

		class domin_struct : public data_field, public parser
		{
		public:
			domin_struct();

			~domin_struct() = default;

		public:
			virtual parse_error visit(std::ifstream& ifs, std::size_t& column, std::size_t& row) override;
		};

	} // namespace lazytool
} // namespace aquarius