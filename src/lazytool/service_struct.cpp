#include "service_struct.h"
#include "domin_parse.h"

namespace aquarius
{
	namespace lazytool
	{
        service_struct::service_struct()
            : parser(struct_type::service)
            , header_ptr_(new domin_struct())
            , body_ptr_(new domin_struct())
        {
        }

        service_struct::parse_error service_struct::visit(std::ifstream& ifs, std::size_t& column, std::size_t& row)
        {
            while (!ifs.eof())
            {
                auto result = seek<'}'>(ifs, column, row);
                if (result == parse_error::success)
                    return result;
                auto name = read_value<token::key, ':'>(ifs, column, row);
                if (name.empty())
                    return parse_error::syntax;

                if (name == "header")
                {
                    result = header_ptr_->visit(ifs, column, row);
                    header_ptr_->set_name(this->name());
                }
                else if (name == "body")
                {
                    result = body_ptr_->visit(ifs, column, row);
                    body_ptr_->set_name(this->name());
                }
                else
                    return parse_error::keyword;
            }
            return ifs.eof() ? parse_error::eof : parse_error::success;
        }

        std::shared_ptr<domin_struct> service_struct::header() const
        {
            return header_ptr_;
        }

        std::shared_ptr<domin_struct> service_struct::body() const
        {
            return body_ptr_;
        }
	}
}