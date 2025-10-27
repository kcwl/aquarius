#include "normal_message_generate.h"
#include "normal_message_parse.h"
#include "domin_parse.h"
#include "domin_generater.h"
#include "structure_parse.h"
#include "router_parse.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{

			generator::generate_error normal_message_generate::visit(std::shared_ptr<parser> parser,
																	 std::ofstream& ofs_h, std::ofstream& ofs_cpp)
			{
				auto msg_ptr = std::dynamic_pointer_cast<normal_message_parse>(parser);
				if (!msg_ptr)
					return generate_error::empty;

				auto domin = std::make_shared<domin_parse>();
				domin->values_.swap(msg_ptr->request_ptr_->values_);
				domin->name_ = msg_ptr->request_ptr_->name_ + "_req_body";
				domin->protocol_type_ = msg_ptr->router_ptr_->name_;

				domin_generate dg{};

				dg.visit(domin, ofs_h, ofs_cpp);

				domin->values_.swap(msg_ptr->response_ptr_->values_);
				domin->name_ = msg_ptr->request_ptr_->name_ + "_resp_body";
				domin->protocol_type_ = msg_ptr->router_ptr_->name_;

				dg.visit(domin, ofs_h, ofs_cpp);

				return generate_error::success;
			}

			void normal_message_generate::defined(std::shared_ptr<parser> parser, std::ofstream& ofs)
			{
				auto ptr = std::dynamic_pointer_cast<normal_message_parse>(parser);

				if (!ptr)
					return;

				if (ptr->router_ptr_->name_ == "tcp")
				{
					ofs << "using " << ptr->name_ << "_request = aquarius::virgo::" << ptr->router_ptr_->name_
						<< "_request<\"" << ptr->router_ptr_->value_ << "\", aquarius::tcp_request_header, "
						<< ptr->name_ << "_req_body>;\n";
					ofs << "using " << ptr->name_ << "_response = aquarius::virgo::" << ptr->router_ptr_->name_
						<< "_response<\"" << ptr->router_ptr_->value_ << "\", aquarius::tcp_response_header, "
						<< ptr->name_ << "_resp_body>;\n";
				}
				else if (ptr->router_ptr_->name_ == "http")
				{
					ofs << "using " << ptr->name_
						<< "_request = aquarius::virgo::http_request<aquarius::virgo::http_method::"
						<< ptr->router_ptr_->mode_ << ",\"" << ptr->router_ptr_->value_ << "\", aquarius::http_request_header, " << ptr->name_ << "_req_body>;\n";
					ofs << "using " << ptr->name_
						<< "_response = aquarius::virgo::http_response<aquarius::virgo::http_method::"
						<< ptr->router_ptr_->mode_ << ",\"" << ptr->router_ptr_->value_ << "\", aquarius::http_response_header, " << ptr->name_ << "_resp_body>;\n";
				}
			}
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius