#include "message_parse.h"
#include "protocol_parse.h"
#include "router_parse.h"

namespace aquarius
{
	namespace lazytool
	{

		message_parse::message_parse()
			: parser(struct_type::message)
			, router_ptr_(new router_parse())
			, request_ptr_(new protocol_parse())
			, response_ptr_(new protocol_parse())
		{}

		parser::parse_error message_parse::visit(std::ifstream& ifs, std::size_t& column, std::size_t& row)
		{
			name_ = read_value<token::value, '-'>(ifs, column, row);

			if (name_.empty())
				return parse_error::syntax;

			auto result = seek<'>'>(ifs, column, row);

			if (result != parse_error::success)
				return result;

			result = router_ptr_->visit(ifs, column, row);

			if (result != parse_error::success)
				return result;

			while (!ifs.eof())
			{
				result = seek<'}'>(ifs, column, row);

				if (result == parse_error::success)
					return result;

				auto sub_key = read_value<token::key, '{'>(ifs, column, row);

				if (sub_key == "request")
				{
					result = request_ptr_->visit(ifs, column, row);

					if (result == parse_error::success)
					{
						request_ptr_->name_ = name_;
					}
				}
				else if (sub_key == "response")
				{
					result = response_ptr_->visit(ifs, column, row);

					if (result == parse_error::success)
					{
						response_ptr_->name_ = name_;
					}
				}
				else
					return parse_error::keyword;
			}

			return parse_error::eof;
		}
	} // namespace lazytool
} // namespace aquarius