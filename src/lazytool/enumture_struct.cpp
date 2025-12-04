#include "enumture_struct.h"

namespace aquarius
{
	namespace lazytool
	{
		enum_struct::enum_struct()
			: parser(struct_type::enumture)
		{}

		enum_struct::parse_error enum_struct::visit(std::ifstream& ifs, std::size_t& column, std::size_t& row)
		{
			auto name = read_value<token::value, '{'>(ifs, column, row);

			if (name.empty())
				return parse_error::syntax;

			this->set_name(name);

			// put_custom_type(name);

			parse_error result{};

			while (!ifs.eof() && result == parse_error::success)
			{
				result = seek<'}'>(ifs, column, row);
				if (result == parse_error::success)
					return result;

				auto v = read_value<token::value, ';'>(ifs, column, row);

				if (v.empty())
					return parse_error::syntax;

				this->add_field("", v);

				result = parse_error::success;
			}

			return result;
		}
	} // namespace lazytool
} // namespace aquarius