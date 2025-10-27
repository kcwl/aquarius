#include "protocol_parse.h"
#include "domin_parse.h"

namespace aquarius
{
	namespace lazytool
	{
        protocol_parse::protocol_parse()
            : header_ptr_(new domin_parse())
            , body_ptr_(new domin_parse())
        {

        }

        std::ostream& protocol_parse::operator<<(std::ostream& os) const
        {
            return os << *header_ptr_ << *body_ptr_;
        }

        parser::parse_error protocol_parse::visit(std::ifstream& ifs, std::size_t& column, std::size_t& row)
        {
            while (!ifs.eof())
            {
                auto result = seek<'}'>(ifs, column, row);
                if (result == parse_error::success)
                    return result;

                name_ = read_value<token::key, ':'>(ifs, column, row);

                if (name_.empty())
                    return parse_error::syntax;

                switch (from_scope_string(name_))
                {
                    case scope::s_header:
                        result = header_ptr_->visit(ifs, column, row);
                        break;
                    case scope::s_body:
                        result = body_ptr_->visit(ifs, column, row);
                        break;
                    default:
                        return parse_error::keyword;
                }
            }

            return parse_error::eof;
        }

        protocol_parse::scope protocol_parse::from_scope_string(const std::string& s)
        {
            if (s == "header")
            {
                return scope::s_header;
            }
            else if (s == "body")
            {
                return scope::s_body;
            }

            return scope::s_error;
        }

        std::ostream& operator<<(std::ostream& os, const protocol_parse& pp)
        {
            pp << os;

            return os;
        }
	}
}