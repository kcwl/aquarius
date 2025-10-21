#include "normal_message_generate.h"
#include "normal_message_parse.h"
#include "domin_parse.h"
#include "domin_generater.h"
#include "structure_parse.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			generator::generate_error normal_message_generate::visit(std::shared_ptr<parser> parser, std::ofstream& ofs_h,
																  std::ofstream& ofs_cpp)
			{
				auto msg_ptr = std::dynamic_pointer_cast<normal_message_parse>(parser);
				if (!msg_ptr)
					return generate_error::empty;

				auto domin = std::make_shared<domin_parse>();
				domin->values_.swap(msg_ptr->request_ptr_->values_);
				domin->name_ = msg_ptr->request_ptr_->name_ + "_req_body";
				domin->protocol_type_ = msg_ptr->protocol_type_;

				domin_generate dg{};

				dg.visit(domin, ofs_h, ofs_cpp);

				domin->values_.swap(msg_ptr->response_ptr_->values_);
				domin->name_ = msg_ptr->request_ptr_->name_ + "_resp_body";
				domin->protocol_type_ = msg_ptr->protocol_type_;

				dg.visit(domin, ofs_h, ofs_cpp);

				return generate_error::success;
			}
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius