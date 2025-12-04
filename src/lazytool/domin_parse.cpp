#include "domin_parse.h"

namespace aquarius
{
	namespace lazytool
	{
		domin_struct::domin_struct()
			: parser(struct_type::domin)
		{
		}

		domin_struct::parse_error domin_struct::visit(std::ifstream& ifs, std::size_t& column, std::size_t& row)
		{
			while (!ifs.eof())
			{
				auto c = ifs.peek();
				if (std::isspace(c))
				{
					ifs.get();
					continue;
				}

				auto k = read_value<token::domin, ' '>(ifs, column, row);
				if (k.empty())
					return parse_error::ending;
				auto v = read_value<token::value, ';'>(ifs, column, row);
				if (v.empty())
					return parse_error::syntax;

				this->add_field(k, v);
			}
			return parse_error::eof;
		}
	} // namespace lazytool
} // namespace aquarius
