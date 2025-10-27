#include "domin_generater.h"
#include "domin_parse.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			domin_generate::generate_error domin_generate::visit(std::shared_ptr<parser> parser, std::ofstream& ofs_h,
																 std::ofstream& ofs_cpp)
			{
				auto ptr = dynamic_pointer_cast<domin_parse>(parser);

				if (!ptr)
					return generate_error::empty;

				generate_main_define(ofs_h, ptr);

				generate_main_src(ofs_cpp, ptr);

				generate_operator_equal_define(ofs_h, ptr);

				generate_operator_stream_define(ofs_h, ptr);

				generate_operator_equal_src(ofs_cpp, ptr);

				generate_operator_stream_src(ofs_cpp, ptr);

				generate_to_tag(ofs_cpp, ptr);

				generate_from_tag(ofs_cpp, ptr);

				return generate_error::success;
			}

			void domin_generate::generate_main_define(std::ofstream& ofs, std::shared_ptr<domin_parse> parser)
			{
				ofs << "\n";
				ofs << "class " << parser->name_ << " : public aquarius::serialize::" << parser->protocol_type_
					<< "_serialize\n";
				ofs << "{\n";
				ofs << "public:\n";
				ofs << "\t" << parser->name_ << "() = default;\n";
				ofs << "\tvirtual ~" << parser->name_ << "() = default;\n";
				ofs << "\n";
				ofs << "\tvirtual void serialize(aquarius::flex_buffer& buffer) override;\n\n";
				ofs << "\tvirtual void deserialize(aquarius::flex_buffer& buffer) override;\n\n";
				ofs << "public:\n";
				for (auto& [type, name] : parser->values_)
				{
					ofs << "\t" << type << " " << name << ";\n";
				}
				ofs << "};\n";
			}

			void domin_generate::generate_main_src(std::ofstream& ofs, std::shared_ptr<domin_parse> parser)
			{
				ofs << "\n";
				ofs << "void " << parser->name_ << "::serialize(aquarius::flex_buffer& buffer)\n";
				ofs << "{\n";

				if (parser->protocol_type_ == "tcp")
				{
					for (auto& [type, name] : parser->values_)
					{
						ofs << "\tthis->parse_to(" << name << ", buffer);\n";
					}
				}
				else
				{
					ofs << "\tthis->parse_to(*this, buffer);\n";
				}
				
				ofs << "}\n";

				ofs << "\n";
				ofs << "void " << parser->name_ << "::deserialize(aquarius::flex_buffer& buffer)\n";
				ofs << "{\n";

				if (parser->protocol_type_ == "tcp")
				{
					for (auto& [type, name] : parser->values_)
					{
						ofs << "\t" << name << " = this->parse_from<" << type << ">(buffer);\n";
					}
				}
				else
				{
					ofs << "\t*this = this->parse_from<" << parser->name_ << ">(buffer);\n";
				}
				
				ofs << "}\n";
			}
		}
	} // namespace lazytool
} // namespace aquarius