#include "enumture_parse.h"

namespace aquarius
{
	namespace lazytool
	{
		enumture_parse::enumture_parse()
			: parser(struct_type::enumture)
		{}

		parser::parse_error enumture_parse::visit(std::ifstream& ifs, std::size_t& column, std::size_t& row)
		{
			name_ = read_value<token::value, '{'>(ifs, column, row);

			if (name_.empty())
				return parse_error::syntax;

			// put_custom_type(name_);

			parse_error result{};

			while (!ifs.eof() && result == parse_error::success)
			{
				result = seek<'}'>(ifs, column, row);
				if (result == parse_error::success)
					return result;

				auto v = read_value<token::value, ';'>(ifs, column, row);

				if (v.empty())
					return parse_error::syntax;

				values_.emplace_back(v, "");

				result = parse_error::success;
			}

			return result;
		}
	} // namespace lazytool
} // namespace aquarius