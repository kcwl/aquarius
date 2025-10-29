#include "structure_parse.h"

namespace aquarius
{
	namespace lazytool
	{
		structure::parse_error structure::visit(std::ifstream& ifs, std::size_t& column, std::size_t& row)
		{
			auto name = read_value<token::value, '{'>(ifs, column, row);
			if (name.empty())
				return parse_error::syntax;
			// put_custom_type(name);
			this->set_name(name);
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

				this->add_field(k, v);
			}
			return ifs.eof() ? parse_error::eof : parse_error::syntax;
		}
	} // namespace lazytool
} // namespace aquarius