#include "router_parse.h"

namespace aquarius
{
	namespace lazytool
	{

		std::ostream& router_parse::operator<<(std::ostream& os) const
		{
			os << "router: " << name_ << std::endl;
			os << "mode: " << mode_ << std::endl;
			os << "value: " << value_ << std::endl;

			return os;
		}

		parser::parse_error router_parse::visit(std::ifstream& ifs, std::size_t& column, std::size_t& row)
		{
			name_ = read_value<token::key, '-'>(ifs, column, row);

			if (!check_protocol_type(name_))
				return parse_error::routers;

			mode_ = read_value<token::key, ':'>(ifs, column, row);

			if (!check_mode(mode_))
				return parse_error::routers_mode;

			value_ = read_value<token::path, '{'>(ifs, column, row);

			if (value_.empty())
				return parse_error::routers_value;

			return parse_error::success;
		}

		bool router_parse::check_protocol_type(const std::string& protocol_type)
		{
			return protocol_type == "tcp" || protocol_type == "http";
		}

		bool router_parse::check_mode(const std::string& mode)
		{
			return mode == "key" || mode == "regex";
		}

		std::ostream& operator<<(std::ostream& os, const router_parse& r)
		{
			r << os;

			return os;
		}
	} // namespace lazytool
} // namespace aquarius
