#include "structure_parse.h"

namespace aquarius
{
	namespace lazytool
	{
		structure_parse::structure_parse()
			: parser(struct_type::structure)
			, anonymous_(false)
		{}

		std::ostream& structure_parse::operator<<(std::ostream& os) const
		{
			os << "class " << name_ << std::endl;
			os << "{\n";
			for (auto& v : values_)
			{
				os << "\t" << v.first << " " << v.second << std::endl;
			}

			os << "};\n";
			return os;
		}

		parser::parse_error structure_parse::visit(std::ifstream& ifs, std::size_t& column, std::size_t& row)
		{
			if (!anonymous_)
			{
				name_ = read_value<token::value, '{'>(ifs, column, row);

				if (name_.empty())
					return parse_error::syntax;

				// put_custom_type(name_);
			}

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

		void structure_parse::set_anonymous(bool value)
		{
			anonymous_ = value;
		}

		std::ostream& operator<<(std::ostream& os, const structure_parse& pr)
		{
			pr << os;

			return os;
		}
	} // namespace lazytool
} // namespace aquarius