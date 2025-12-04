#include "protocol_generater.h"
#include "protocol_parse.h"
#include <format>
#include "service_struct.h"
#include "domin_parse.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{

			protocol_generate::generate_error protocol_generate::visit(std::shared_ptr<parser> parser,
																	   std::ofstream& ofs_h, std::ofstream& ofs_cpp)
			{
				auto ptr = std::dynamic_pointer_cast<protocol_struct>(parser);

				if (!ptr)
					return generater::generate_error::empty;

				generate_service_define(ofs_h, "request", "header", ptr->request(),
										ptr->protocol() + "_" + ptr->method());

				generate_service_define(ofs_h, "response", "header", ptr->response(),
										ptr->protocol() + "_" + ptr->method());

				generate_service_src(ofs_cpp, "request", "body", ptr->request(), ptr->protocol(), ptr->method());

				generate_service_src(ofs_cpp, "response", "body", ptr->response(), ptr->protocol(), ptr->method());

				ptr->request()->header()->set_name(ptr->request()->name() + "_request_header");
				ptr->request()->body()->set_name(ptr->request()->name() + "_request_body");

				ptr->response()->header()->set_name(ptr->response()->name() + "_response_header");
				ptr->response()->body()->set_name(ptr->response()->name() + "_response_body");

				generate_operator_equal_define(ofs_h, ptr->request()->header());

				generate_operator_stream_define(ofs_h, ptr->request()->header());

				generate_operator_equal_src(ofs_cpp, ptr->request()->header());

				generate_operator_stream_src(ofs_cpp, ptr->request()->header());

				generate_operator_equal_define(ofs_h, ptr->request()->body());

				generate_operator_stream_define(ofs_h, ptr->request()->body());

				generate_operator_equal_src(ofs_cpp, ptr->request()->body());

				generate_operator_stream_src(ofs_cpp, ptr->request()->body());

				generate_operator_equal_define(ofs_h, ptr->response()->header());

				generate_operator_stream_define(ofs_h, ptr->response()->header());

				generate_operator_equal_src(ofs_cpp, ptr->response()->header());

				generate_operator_stream_src(ofs_cpp, ptr->response()->header());

				generate_operator_equal_define(ofs_h, ptr->response()->body());

				generate_operator_stream_define(ofs_h, ptr->response()->body());

				generate_operator_equal_src(ofs_cpp, ptr->response()->body());

				generate_operator_stream_src(ofs_cpp, ptr->response()->body());


				generate_to_tag_define(ofs_h, ptr->request()->header());

				generate_from_tag_define(ofs_h, ptr->request()->header());

				generate_to_tag_define(ofs_h, ptr->request()->body());

				generate_from_tag_define(ofs_h, ptr->request()->body());

				generate_to_tag_define(ofs_h, ptr->response()->header());

				generate_from_tag_define(ofs_h, ptr->response()->header());

				generate_to_tag_define(ofs_h, ptr->response()->body());

				generate_from_tag_define(ofs_h, ptr->response()->body());


				generate_to_tag(ofs_cpp, ptr->request()->header());

				generate_from_tag(ofs_cpp, ptr->request()->header());

				generate_to_tag(ofs_cpp, ptr->request()->body());

				generate_from_tag(ofs_cpp, ptr->request()->body());

				generate_to_tag(ofs_cpp, ptr->response()->header());

				generate_from_tag(ofs_cpp, ptr->response()->header());

				generate_to_tag(ofs_cpp, ptr->response()->body());

				generate_from_tag(ofs_cpp, ptr->response()->body());

				return generate_error::success;
			}

			void protocol_generate::defined(std::shared_ptr<parser> parser, std::ofstream& ofs)
			{
				auto ptr = std::dynamic_pointer_cast<protocol_struct>(parser);

				if (!ptr)
					return;

				if (ptr->protocol() == "tcp")
				{
					ofs << "using " << ptr->name() << "_request = aquarius::virgo::tcp_request<\"" << ptr->router_key()
						<< "\", " << ptr->name() << "_request_header, " << ptr->name() << "_request_body>;" << CRLF;
					ofs << "using " << ptr->name() << "_response = aquarius::virgo::tcp_response<\"" << ptr->router_key() << "\", " << ptr->name() << "_response_header, "
						<< ptr->name() << "_response_body>;" << CRLF;
				}
				else if (ptr->protocol() == "http")
				{
					ofs << "using " << ptr->name()
						<< "_request = aquarius::virgo::http_request<" << "\"" << ptr->router_key() << "\", " << ptr->name() << "_request_header, " << ptr->name()
						<< "_request_body>;" << CRLF;
					ofs << "using " << ptr->name()
						<< "_response = aquarius::virgo::http_response<" << "\"" << ptr->router_key() << "\", " << ptr->name() << "_response_header, " << ptr->name()
						<< "_response_body>;" << CRLF;
				}
			}

			protocol_generate::generate_error protocol_generate::generate_service_define(
				std::ofstream& ofs, const std::string& service, const std::string& domin, std::shared_ptr<service_struct> parser,
				const std::string& protocol_router_key)
			{
				auto f = [&](const std::string& cla_name, const auto& fields)
				{
					ofs << "class " << cla_name << " : public aquarius::" << protocol_router_key
						<< "_serialize" << CRLF;
					ofs << "{" << CRLF;
					ofs << "public:" << CRLF;
					ofs << "\t" << cla_name << "() = default;" << CRLF;
					ofs << "\tvirtual ~" << cla_name << "() = default;" << CRLF;
					ofs << CRLF;
					ofs << "\tvirtual void serialize(aquarius::flex_buffer& buffer) override;" << TWO_CRLF;
					ofs << "\tvirtual void deserialize(aquarius::flex_buffer& buffer) override;" << TWO_CRLF;
					ofs << "public:" << CRLF;
					for (auto& [type, name] : fields)
					{
						ofs << "\t" << type << " " << name << ";" << CRLF;
					}

					ofs << "};" << CRLF;
				};

				f(std::format("{}_{}_header", parser->name(), service), parser->header()->fields());
				f(std::format("{}_{}_body", parser->name(), service), parser->body()->fields());

				return generate_error::success;
			}

			protocol_generate::generate_error protocol_generate::generate_service_src(
				std::ofstream& ofs, const std::string& service, const std::string& domin, std::shared_ptr<service_struct> parser,
				const std::string& protocol, const std::string& router_key)
			{
		
				auto f = [&](const std::string& cla_name, const auto& fields)
				{
					ofs << CRLF;
					ofs << "void " << cla_name << "::serialize(aquarius::flex_buffer& buffer)" << CRLF;
					ofs << "{" << CRLF;

					if (protocol == "tcp")
					{
						for (auto& [type, name] : fields)
						{
							ofs << "\tthis->parse_to(" << name << ", buffer);" << CRLF;
						}
					}
					else
					{
						ofs << "\tthis->parse_to(*this, buffer);" << CRLF;
					}

					ofs << "}" << CRLF;

					ofs << CRLF;
					ofs << "void " << cla_name << "::deserialize(aquarius::flex_buffer& buffer)" << CRLF;
					ofs << "{" << CRLF;

					if (protocol == "http")
					{
						if (router_key == "get")
						{
							for (auto& [type, name] : fields)
							{
								ofs << "\t" << name << " = this->parse_from<" << type << ">(buffer, \"" << name << "\");" << CRLF;
							}
						}
						else
						{
							ofs << "\t*this = this->parse_from<" << cla_name << ">(buffer);" << CRLF;
						}
						
					}
					else
					{
						for (auto& [type, name] : fields)
						{
							ofs << "\t" << name << " = this->parse_from<" << type << ">(buffer);" << CRLF;
						}
					}

					ofs << "}" << CRLF;
				};

				f(std::format("{}_{}_header", parser->name(), service), parser->header()->fields());
				f(std::format("{}_{}_body", parser->name(), service), parser->body()->fields());

				return generate_error::success;
			}
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius