#include "message_generater.h"
#include "message_parse.h"
#include <format>

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{

			generator::generate_error message_generate::visit(std::shared_ptr<parser> parser, std::ofstream& ofs_h,
															  std::ofstream& ofs_cpp)
			{
				auto ptr = dynamic_pointer_cast<message_struct>(parser);
				if (!ptr)
					return generate_error::empty;

				generate_main_define(ofs_h, ptr);

				generate_main_src(ofs_cpp, ptr);

				defined(parser, ofs_h);

				ptr->set_name(ptr->name() + "_body");

				generate_operator_equal_define(ofs_h, ptr);

				generate_operator_stream_define(ofs_h, ptr);

				generate_operator_equal_src(ofs_cpp, ptr);

				generate_operator_stream_src(ofs_cpp, ptr);

				generate_to_tag_define(ofs_h, ptr);

				generate_from_tag_define(ofs_h, ptr);

				generate_to_tag(ofs_cpp, ptr);

				generate_from_tag(ofs_cpp, ptr);

				ptr->set_name(ptr->name().substr(0, ptr->name().size() - 5));

				return generate_error::success;
			}

			void message_generate::defined(std::shared_ptr<parser> parser, std::ofstream& ofs)
			{
				auto ptr = std::dynamic_pointer_cast<message_struct>(parser);

				if (!ptr)
					return;

				auto service = ptr->name().substr(ptr->name().find_last_of('_'));

				if (ptr->protocol() == "tcp")
				{
					ofs << "using " << ptr->name() << " = aquarius::virgo::" << ptr->protocol()

						<< service << "<\"" << ptr->router_key() << "\", aquarius::tcp" << service << "_header, "
						<< ptr->name() << "_body>;\n";
				}
				else if (ptr->protocol() == "http")
				{
					ofs << "using " << ptr->name() << " = aquarius::virgo::http" << service
						<< "<aquarius::virgo::http_method::" << ptr->method() << ",\"" << ptr->router_key()
						<< "\", aquarius::http" << service << "_header, " << ptr->name() << "_body>;\n";
				}
			}

			message_generate::generate_error message_generate::generate_main_define(
				std::ofstream& ofs, std::shared_ptr<message_struct> parser)
			{
				auto class_name = std::format("{}_body", parser->name());

				auto parent_name = std::format("{}_{}_serialize", parser->protocol(), parser->method());

				ofs << "class " << class_name << " : public aquarius::" << parent_name << "\n";
				ofs << "{\n";
				ofs << "public:\n";
				ofs << "\t" << class_name << "() = default;\n";
				ofs << "\tvirtual ~" << class_name << "() = default;\n";
				ofs << "\n";
				ofs << "\tvirtual void serialize(aquarius::flex_buffer& buffer) override;\n\n";
				ofs << "\tvirtual void deserialize(aquarius::flex_buffer& buffer) override;\n\n";
				ofs << "public:\n";
				for (auto& [type, name] : parser->fields())
				{
					ofs << "\t" << type << " " << name << ";\n";
				}
				ofs << "};\n";

				return generate_error::success;
			}

			message_generate::generate_error message_generate::generate_main_src(std::ofstream& ofs,
																				 std::shared_ptr<message_struct> parser)
			{
				auto class_name = std::format("{}_body", parser->name());

				ofs << "\n";
				ofs << "void " << class_name << "::serialize(aquarius::flex_buffer& buffer)\n";
				ofs << "{\n";

				if (parser->protocol() == "http" )
				{
					if (parser->method() == "get")
					{
						for (auto& [type, name] : parser->fields())
						{
							ofs << "\tthis->parse_to(" << name << ", buffer, \"" << name <<"\");\n";
						}
					}
					else
					{
						ofs << "\tthis->parse_to(*this, buffer);\n";
					}
				}
				else
				{
					for (auto& [type, name] : parser->fields())
					{
						ofs << "\tthis->parse_to(" << name << ", buffer);\n";
					}
				}

				ofs << "}\n";

				ofs << "\n";
				ofs << "void " << class_name << "::deserialize(aquarius::flex_buffer& buffer)\n";
				ofs << "{\n";

				if (parser->protocol() == "http" && parser->method() == "post")
				{
					ofs << "\t*this = this->parse_from<" << class_name << ">(buffer);\n";
				}
				else
				{
					for (auto& [type, name] : parser->fields())
					{
						ofs << "\t" << name << " = this->parse_from<" << type << ">(buffer);\n";
					}
				}

				ofs << "}\n";

				return generate_error::success;
			}
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius