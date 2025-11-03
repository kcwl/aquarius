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
						<< ptr->name() << "_body>;" << CRLF;
				}
				else if (ptr->protocol() == "http")
				{
					ofs << "using " << ptr->name() << " = aquarius::virgo::http" << service << "<\""
						<< ptr->router_key() << "\", aquarius::http" << service << "_header, " << ptr->name()
						<< "_body>;" << CRLF;
				}
			}

			message_generate::generate_error message_generate::generate_main_define(
				std::ofstream& ofs, std::shared_ptr<message_struct> parser)
			{
				auto class_name = std::format("{}_body", parser->name());

				auto parent_name = std::format("{}_{}_serialize", parser->protocol(), parser->method());

				ofs << "class " << class_name << " : public aquarius::" << parent_name << CRLF;
				ofs << "{" << CRLF;
				ofs << "public:" << CRLF;
				ofs << "\t" << class_name << "() = default;" << CRLF;
				ofs << "\tvirtual ~" << class_name << "() = default;" << CRLF;
				ofs << CRLF;
				ofs << "\tvirtual void serialize(aquarius::flex_buffer& buffer) override;" << TWO_CRLF;
				ofs << "\tvirtual void deserialize(aquarius::flex_buffer& buffer) override;" << TWO_CRLF;
				ofs << "public:" << CRLF;
				for (auto& [type, name] : parser->fields())
				{
					ofs << "\t" << type << " " << name << ";" << CRLF;
				}
				ofs << "};" << CRLF;

				return generate_error::success;
			}

			message_generate::generate_error message_generate::generate_main_src(std::ofstream& ofs,
																				 std::shared_ptr<message_struct> parser)
			{
				auto class_name = std::format("{}_body", parser->name());

				ofs << CRLF;
				ofs << "void " << class_name << "::serialize(aquarius::flex_buffer& buffer)" << CRLF;
				ofs << "{" << CRLF;

				auto f = [&](const std::string& method, const std::string& direction, bool has_type = false)
				{
					if (method == "binary")
					{
						for (auto& [type, name] : parser->fields())
						{
							if (has_type)
							{
								ofs << "\t" << name << " = this->parse_" << direction << "<" << type << ">(buffer);"
									<< CRLF;
							}
							else
							{
								ofs << "\tthis->parse_" << direction << "(" << name << ", buffer);" << CRLF;
							}
						}
					}
					else if (method == "kv")
					{
						bool start = true;
						for (auto& [type, name] : parser->fields())
						{
							if (!has_type)
							{
								if (start)
								{
									start = !start;
									ofs << "\tbuffer.put('?');" << CRLF;
								}
								else
									ofs << "\tbuffer.put('&');" << CRLF;
							}

							if (has_type)
							{
								ofs << "\t" <<name << " = this->parse_" << direction << "<" << type << ">(buffer, \"" << name << "\");"
									<< CRLF;
							}
							else
							{
								ofs << "\tthis->parse_" << direction << "(" << name << ", buffer, \"" << name << "\");"
									<< CRLF;
							}
						}

						ofs.seekp(-1, std::ios::cur);
						ofs << " ";
					}
					else if (method == "json")
					{
						if (has_type)
						{
							ofs << "\t*this = this->parse_" << direction <<"<" << class_name << ">(buffer); " << CRLF;
						}
						else
						{
							ofs << "\tthis->parse_" << direction << "(*this, buffer);" << CRLF;
						}
					}
					else
					{
					}
				};

				f(parser->method(), "to");

				ofs << "}" << CRLF;

				ofs << CRLF;
				ofs << "void " << class_name << "::deserialize(aquarius::flex_buffer& buffer)" << CRLF;
				ofs << "{" << CRLF;

				f(parser->method(), "from", true);

				ofs << "}" << CRLF;

				return generate_error::success;
			}
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius