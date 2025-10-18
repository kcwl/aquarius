#include "structure_parse.h"

namespace aquarius
{
	namespace lazytool
	{
		structure_parse::structure_parse()
			: parser(struct_type::structure)
		{}

		parser::parse_error structure_parse::visit(std::ifstream& ifs, std::size_t& column, std::size_t& row)
		{
			name_ = read_value<token::value, '{'>(ifs, column, row);

			if (name_.empty())
				return parse_error::syntax;

			// put_custom_type(name_);

			while (!ifs.eof())
			{
				auto res = seek<'}'>(ifs, column, row);
				if (res == parse_error::success)
					return res;

				auto k = read_value<token::key, ' '>(ifs, column, row);

				if (k.empty())
					return parse_error::syntax;

				auto v = read_value<token::value, ';'>(ifs, column, row);
				if (v.empty())
					return parse_error::syntax;

				this->values_.emplace_back(k, v);
			}

			return ifs.eof() ? parse_error::eof : parse_error::syntax;
		}
	} // namespace lazytool
} // namespace aquarius