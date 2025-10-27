#include "protocol_generater.h"
#include "domin_generater.h"
#include "protocol_parse.h"
#include "domin_parse.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			protocol_generate::generate_error protocol_generate::visit(std::shared_ptr<parser> parser, std::ofstream& ofs_h,
																	   std::ofstream& ofs_cpp)
			{
				auto ptr = std::dynamic_pointer_cast<protocol_parse>(parser);

				if (!ptr)
					return generate_error::empty;

				ptr->header_ptr_->name_ = ptr->name_ + "_header";

				ptr->body_ptr_->name_ = ptr->name_ + "_body";

				ptr->header_ptr_->protocol_type_ = ptr->protocol_type_;

				ptr->body_ptr_->protocol_type_ = ptr->protocol_type_;

				domin_generate().visit(ptr->header_ptr_, ofs_h, ofs_cpp);

				domin_generate().visit(ptr->body_ptr_, ofs_h, ofs_cpp);

				return generate_error::success;
			}
		}
	} // namespace lazytool
} // namespace aquarius