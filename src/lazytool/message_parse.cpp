#include "message_parse.h"
#include "router_field.h"

namespace aquarius
{
	namespace lazytool
	{
        message_struct::message_struct()
            : parser(struct_type::message)
            , router_ptr_(new router_field())
        {

        }

        message_struct::parse_error message_struct::visit(std::ifstream& ifs, std::size_t& column, std::size_t& row)
        {
            auto name = read_value<token::value, '-'>(ifs, column, row);

            if (name.empty())
                return parse_error::syntax;

            this->set_name(name);

            auto result = seek<'>'>(ifs, column, row);
            if (result != parse_error::success)
                return result;

            result = router_ptr_->visit(ifs, column, row);

            if (result != parse_error::success)
                return result;

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

        std::string message_struct::protocol() const
        {
            return router_ptr_->protocol();
        }

        std::string message_struct::method() const
        {
            return router_ptr_->method();
        }

        std::string message_struct::router_key() const
        {
            return router_ptr_->key();
        }
	} // namespace lazytool
} // namespace aquarius