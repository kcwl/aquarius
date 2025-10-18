#include "domin_parse.h"

namespace aquarius
{
	namespace lazytool
	{
		domin_parse::domin_parse()
			: parser(struct_type::domin)
		{}

		std::ostream& domin_parse::operator<<(std::ostream& os) const
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

		parser::parse_error domin_parse::visit(std::ifstream& ifs, std::size_t& column, std::size_t& row)
		{
			while (!ifs.eof())
			{
				auto k = read_value<token::domin, ' '>(ifs, column, row);

				if (k.empty())
					return parse_error::ending;

				auto v = read_value<token::value, ';'>(ifs, column, row);

				if (v.empty())
					return parse_error::syntax;

				values_.emplace_back(k, v);
			}

			return parse_error::eof;
		}

		std::string domin_parse::read_domain(std::ifstream& ifs, std::size_t& column, std::size_t& row)
		{
			std::string value{};

			while (!ifs.eof())
			{
				auto c = ifs.get();

				if (std::iscntrl(c))
					continue;

				if (!std::isalnum(c))
					return value;

				value.push_back(static_cast<char>(c));
			}

			return {};
		}

		std::ostream& operator<<(std::ostream& os, const domin_parse& dp)
		{
			dp << os;

			return os;
		}

	} // namespace lazytool
} // namespace aquarius

