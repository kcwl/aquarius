#include "message_generater.h"
#include "message_parse.h"
#include "protocol_generater.h"
#include "protocol_parse.h"
#include "router_parse.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			message_generate::generate_error message_generate::visit(std::shared_ptr<parser> parser,
																	 std::ofstream& ofs_h, std::ofstream& ofs_cpp)
			{
				auto ptr = std::dynamic_pointer_cast<message_parse>(parser);

				if (!ptr)
					return generater::generate_error::empty;

				ptr->request_ptr_->name_.append("_req");

				ptr->response_ptr_->name_.append("_resp");

				ptr->request_ptr_->protocol_type_ = ptr->router_ptr_->name_;

				ptr->response_ptr_->protocol_type_ = ptr->router_ptr_->name_;

				protocol_generate().visit(ptr->request_ptr_, ofs_h, ofs_cpp);

				protocol_generate().visit(ptr->response_ptr_, ofs_h, ofs_cpp);

				return generate_error::success;
			}

			void message_generate::defined(std::shared_ptr<parser> parser, std::ofstream& ofs)
			{
				auto ptr = std::dynamic_pointer_cast<message_parse>(parser);

				if (!ptr)
					return;

				ofs << "using " << ptr->name_ << "_request = aquarius::virgo::" << ptr->router_ptr_->name_
					<< "_request<" << ptr->router_ptr_->mode_ << ",\"" << ptr->router_ptr_->value_ << "\", "
					<< ptr->name_ << "_req_header, " << ptr->name_ << "_req_body>;\n";
				ofs << "using " << ptr->name_ << "_response = aquarius::virgo::" << ptr->router_ptr_->name_
					<< "_response<" << ptr->router_ptr_->mode_ << ",\"" << ptr->router_ptr_->value_ << "\", "
					<< ptr->name_ << "_resp_header, " << ptr->name_ << "_resp_body>;\n";
			}
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius