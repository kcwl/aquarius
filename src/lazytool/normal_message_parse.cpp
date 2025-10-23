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

		std::ostream& normal_message_parse::operator<<(std::ostream& os) const
		{
			os << "message: " << name_ << std::endl;
			os << "{\n";
			os << "router:" << this->router_ptr_->name_ << ",mode:" << this->router_ptr_->mode_
			   << ",value:" << this->router_ptr_->value_ << std::endl;
			os << "request:" << std::endl;
			for (auto& v : this->request_ptr_->values_)
			{
				os << "\t" << v.first << ":" << v.second << std::endl;
			}
			os << "response:" << std::endl;
			for (auto& v : this->response_ptr_->values_)
			{
				os << "\t" << v.first << ":" << v.second << std::endl;
			}
			os << "}\n";
			return os;
		};

		std::ostream& operator<<(std::ostream& os, const normal_message_parse& m)
		{
			m << os;
			return os;
		}

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

			while (!ifs.eof())
			{
				result = seek<'}'>(ifs, column, row);

				if (result == parse_error::success)
					return result;

				auto sub_key = read_value<token::key, '{'>(ifs, column, row);

				if (sub_key == "request")
				{
					request_ptr_->set_anonymous();

					result = request_ptr_->visit(ifs, column, row);

					if (result == parse_error::success)
					{
						request_ptr_->name_ = name_;
					}
				}
				else if (sub_key == "response")
				{
					response_ptr_->set_anonymous();

					result = response_ptr_->visit(ifs, column, row);

					if (result == parse_error::success)
					{
						response_ptr_->name_ = name_;
					}
				}
				else
					return parse_error::keyword;
			}

			return parse_error::success;
		}
	} // namespace lazytool
} // namespace aquarius