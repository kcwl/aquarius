#include "normal_message_parse.h"
#include "router_parse.h"
#include "structure_parse.h"

namespace aquarius
{
	namespace lazytool
	{

		normal_message_parse::normal_message_parse()
			: parser(struct_type::normal_message)
			, router_ptr_(std::make_shared<router_parse>())
			, request_ptr_(std::make_shared<structure_parse>())
			, response_ptr_(std::make_shared<structure_parse>())
		{}

		parser::parse_error normal_message_parse::visit(std::ifstream& ifs, std::size_t& column, std::size_t& row)
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

			request_ptr_->set_anonymous();

			request_ptr_->visit(ifs, column, row);

			response_ptr_->set_anonymous();

			response_ptr_->visit(ifs, column, row);

			return parse_error::success;
		}
	} // namespace lazytool
} // namespace aquarius