#include "cpp_generater.h"
#include <string_view>
#include <format>
#include "custom_type.h"

using namespace std::string_view_literals;

namespace
{
	constexpr static auto json_serialize_inheritance = "aquarius::http_json_serialize"sv;
	constexpr static auto kv_serialize_inheritance = "aquarius::http_kv_serialize"sv;
	constexpr static auto binary_serialize_inheritance = "aquarius::tcp_binary_serialize"sv;

	constexpr static auto req_body_suffix = "_req_body"sv;
	constexpr static auto resp_body_suffix = "_resp_body"sv;
} // namespace

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			data_field_generator::data_field_generator(std::ofstream& ofs, std::shared_ptr<data_field> data_field_ptr, bool has_end)
				: data_field_ptr_(data_field_ptr)
				, has_end_(has_end)
				, ofs_(ofs)
			{}

			data_field_generator::~data_field_generator()
			{
				if (has_end_)
				{
					ofs_ << "};" << std::endl;
				}
			}

			void data_field_generator::generate()
			{
				generate_header();
				ofs_ << std::endl << "{" << std::endl;

				if (data_field_ptr_->type() == struct_type::structure)
				{
					generate_equal_define();
				}

				ofs_ << std::endl;

				char end = ';';
				if (data_field_ptr_->type() == struct_type::enumture)
					end = ',';
				generate_member_variable_define(end);
			}

			void data_field_generator::generate_header()
			{
				switch (data_field_ptr_->type())
				{
				case struct_type::structure:
					ofs_ << "struct ";
					break;
				case struct_type::enumture:
					ofs_ << "enum ";
					break;
				case struct_type::message:
					ofs_ << "class ";
					break;
				default:
					return;
				}

				ofs_ << data_field_ptr_->name();
			}

			void data_field_generator::generate_member_variable_define(char end)
			{
				for (auto& [type, name] : data_field_ptr_->fields())
				{
					ofs_ << "\t" << type << " " << name << end << std::endl;
				}
			}

			void data_field_generator::scope_public()
			{
				ofs_ << "public:" << std::endl;
			}

			void data_field_generator::scope_private()
			{
				ofs_ << "private:" << std::endl;
			}

			void data_field_generator::scope_protected()
			{
				ofs_ << "protected:" << std::endl;
			}

			void data_field_generator::generate_equal_define()
			{
				ofs_ << "\tbool operator==(const " << data_field_ptr_->name() << " & other) const; " << std::endl;
			}

			void data_field_generator::generate_equal_src()
			{
				ofs_ << std::endl;

				ofs_ << "bool " << data_field_ptr_->name() << "::operator==(const " << data_field_ptr_->name() << "& other) const" << std::endl;
				ofs_ << "{" << std::endl;

				bool start = false;

				auto start_sp = "";

				ofs_ << "\treturn ";

				if (data_field_ptr_->fields().empty())
				{
					ofs_ << "true";
				}
				else
				{
					for (auto& s : data_field_ptr_->fields())
					{
						ofs_ << start_sp << s.second << " == other." << s.second;

						if (!start)
						{
							start_sp = " && ";
							start = !start;
						}
					}
				}

				ofs_ << ";" << std::endl;
				ofs_ << "}" << std::endl;
			}

			void data_field_generator::generate_stream_define()
			{
				ofs_ << "std::ostream& operator<<(std::ostream& os, const " << data_field_ptr_->name() << "& other);\n";
			}

			void data_field_generator::generate_stream_src()
			{
				ofs_ << std::endl;
				ofs_ << "std::ostream& operator<<(std::ostream& os, const " << data_field_ptr_->name() << "& other)\n";
				ofs_ << "{\n";
				if(!data_field_ptr_->fields().empty())
					ofs_ << "\tos ";

				for (auto& [type, name] : data_field_ptr_->fields())
				{
					ofs_ << "<< ";

					if (generator_array_stream(type, name))
						continue;

					generator_normal_stream(name);
				}

				//ofs.seekp(-4, std::ios::cur);

				if (!data_field_ptr_->fields().empty())
					ofs_ << ";\n";

				ofs_ << "\treturn os;\n";

				ofs_ << "}\n";
			}

			void data_field_generator::generate_to_tag(std::shared_ptr<data_field> parser)
			{
				ofs_ << std::endl;

				ofs_ << parser->name() << " tag_invoke(const aquarius::json::value_to_tag<" << parser->name()
					<< ">&, const aquarius::json::value& jv)" << std::endl;
				ofs_ << "{" << std::endl;
				ofs_ << "\t" << parser->name() << " result{};" << std::endl;
				ofs_ << "\tauto obj = jv.try_as_object();" << std::endl;
				ofs_ << "\tif(obj->empty())" << std::endl;
				ofs_ << "\t\treturn {};" << std::endl;

				for (auto& [key, value] : parser->fields())
				{
					if (generate_to_int(key, value))
						continue;

					if (generate_to_string(key, value))
						continue;

					if (generate_to_array(key, value))
						continue;

					generate_to_object(key, value);
				}

				ofs_ << "\treturn result;" << std::endl;

				ofs_ << "}" << std::endl;
			}

			void data_field_generator::generate_from_tag(std::shared_ptr<data_field> parser)
			{
				ofs_ << std::endl;
				ofs_ << "void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const "
					<< parser->name() << "& local)" << std::endl;
				ofs_ << "{" << std::endl;
				ofs_ << "\tauto& jv_obj = jv.emplace_object();" << std::endl;

				for (auto& [key, value] : parser->fields())
				{
					if (generate_from_int(key, value))
						continue;

					if (generate_from_string(key, value))
						continue;

					if (generate_from_array(key, value))
						continue;

					generate_from_object(key, value);
				}
				ofs_ << "}" << std::endl;
			}

			void data_field_generator::generate_json_from_define(std::shared_ptr<data_field> data_ptr)
			{
				if (!data_ptr)
					return;

				ofs_ << "void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const "
					<< data_ptr->name() << "& local);" << std::endl;

				for (auto& [type, _] : data_ptr->fields())
				{
					auto next_data_ptr = get_custom_type(type);
					if (next_data_ptr && !next_data_ptr->has_generate_json)
					{
						generate_json_from_define(next_data_ptr->data_ptr);

						custom_generate_json(type);
					}
				}
			}

			void data_field_generator::generate_json_to_define(std::shared_ptr<data_field> data_ptr)
			{
				if (!data_ptr)
					return;

				ofs_ << data_ptr->name() << " tag_invoke(const aquarius::json::value_to_tag<" << data_ptr->name()
					<< ">&, const aquarius::json::value& jv);" << std::endl;

				for (auto& [type, _] : data_ptr->fields())
				{
					auto next_data_ptr = get_custom_type(type);
					if (next_data_ptr && !next_data_ptr->has_generate_json)
					{
						generate_json_to_define(next_data_ptr->data_ptr);

						custom_generate_json(type);
					}
				}
			}

			bool data_field_generator::generator_array_stream(const std::string& type, const std::string& name)
			{
				if (!type.contains("bytes"))
					return false;

				ofs_ << "\"[\";\n";
				ofs_ << "\tfor (auto& s : other." << name << ")\n";
				ofs_ << "\t{\n";
				ofs_ << "\t\tos << s << \", \";\n";
				ofs_ << "\t}\n";

				ofs_ << "\tos.seekp(-2, std::ios::cur);\n";
				ofs_ << "\tos << \"]\";\n";

				return true;
			}

			void data_field_generator::generator_normal_stream(const std::string& name)
			{
				ofs_ << "other." << name;
			}


			bool data_field_generator::generate_to_int(const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::integer)
					return false;

				if (type == "int32" || type == "uint32" || type == "float" || type == "uint64")
				{
					ofs_ << "\tresult." << value << " = static_cast<" << type << ">(obj->at(\"" << value << "\").as_";

					if (type == "int32" || type == "uint32" || type == "uint64")
						ofs_ << "int64";
					else if (type == "float")
						ofs_ << "double";

					ofs_ << "());" << std::endl;
				}
				else if (type == "int64" || type == "double" || type == "bool")
				{
					ofs_ << "\tresult." << value << " = obj->at(\"" << value << "\").as_" << type << "();" << std::endl;
				}

				return true;
			}

			bool data_field_generator::generate_to_string(const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::string)
					return false;

				ofs_ << "\tresult." << value << " = static_cast<" << type << ">(obj->at(\"" << value
					<< "\").as_string());" << std::endl;

				return true;
			}

			bool data_field_generator::generate_to_array(const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::array)
					return false;

				ofs_ << "\tresult." << value << " = aquarius::json_value_to_array(obj->at(\"" << value << "\"));"
					<< std::endl;

				return true;
			}

			bool data_field_generator::generate_to_object(const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::object)
					return false;

				ofs_ << "\tresult." << value << " = aquarius::json::value_to<" << type << ">(obj->at(\"" << value
					<< "\"));" << std::endl;

				return true;
			}

			bool data_field_generator::generate_from_int(const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::integer)
					return false;

				ofs_ << "\tjv_obj.emplace(\"" << value << "\", local." << value << ");" << std::endl;

				return true;
			}

			bool data_field_generator::generate_from_string(const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::string)
					return false;

				ofs_ << "\tjv_obj.emplace(\"" << value << "\", local." << value << ");" << std::endl;

				return true;
			}

			bool data_field_generator::generate_from_array(const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::array)
					return false;

				ofs_ << "\tjv_obj.emplace(\"" << value << "\", aquarius::json_value_from_array(local." << value << "));"
					<< std::endl;

				return true;
			}

			bool data_field_generator::generate_from_object(const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::object)
					return false;

				ofs_ << "\tjv_obj.emplace(\"" << value << "\", aquarius::json_value_from_object<" << type << ">(local."
					<< value << "));" << std::endl;

				return true;
			}


			data_field_generator::json_type data_field_generator::check_type(const std::string& type)
			{
				if (type == "int32" || type == "uint32" || type == "int64" || type == "uint64" || type == "bool" ||
					type == "float" || type == "double")
				{
					return json_type::integer;
				}
				else if (type == "string")
				{
					return json_type::string;
				}
				else if (type == "bytes")
				{
					return json_type::array;
				}

				return json_type::object;
			}

			message_field_generator::message_field_generator(std::ofstream& ofs,
															 std::shared_ptr<message_field> message_field_ptr)
				: message_field_ptr_(message_field_ptr)
				, ofs_(ofs)
			{}

			void message_field_generator::generate()
			{
				message_field_ptr_->request()->set_name(message_field_ptr_->name() + req_body_suffix.data());

				message_field_ptr_->response()->set_name(message_field_ptr_->name() + resp_body_suffix.data());

				generate_protocol_define(message_field_ptr_->request(), message_field_ptr_->method());

				ofs_ << std::endl;

				generate_protocol_define(message_field_ptr_->response(), message_field_ptr_->method(), true);
			}

			void message_field_generator::generate_protocol_define(std::shared_ptr<data_field> data_field_ptr,
																   const std::string& method, bool has_response)
			{
				data_field_generator gen(ofs_, data_field_ptr);

				gen.generate_header();

				generate_inheritance_serialize_define(method, has_response);

				ofs_ << "{" << std::endl;

				gen.scope_public();
				generate_construction_define(data_field_ptr->name(), !data_field_ptr->fields().empty());

				gen.scope_public();
				gen.generate_equal_define();

				ofs_ << std::endl;

				gen.scope_public();
				generate_serialize_method_define();

				gen.scope_public();
				gen.generate_member_variable_define(';');
			}

			void message_field_generator::generate_protocol_alias_define()
			{
				generate_proto_request_define();

				generate_proto_response_define();
			}

			void message_field_generator::generate_protocol_src()
			{
				generate_construction_src(message_field_ptr_->request(),
										  !message_field_ptr_->request()->fields().empty());

				data_field_generator(ofs_, message_field_ptr_->request(), false).generate_equal_src();

				generate_serialize_method_src(message_field_ptr_->request());

				generate_construction_src(message_field_ptr_->response(),
										  !message_field_ptr_->response()->fields().empty());

				data_field_generator(ofs_, message_field_ptr_->response(), false).generate_equal_src();

				generate_serialize_method_src(message_field_ptr_->response(), message_field_ptr_->method() == "get");
			}

			void message_field_generator::generate_proto_request_define()
			{
				ofs_ << "using " << message_field_ptr_->name() << "_" << message_field_ptr_->protocol()
					 << "_request = aquarius::virgo::" << message_field_ptr_->protocol() << "_request<\"" << message_field_ptr_->router()
					 << "\", ";

				if (message_field_ptr_->protocol() == "http")
				{
					ofs_ << "aquarius::virgo::http_method::" << message_field_ptr_->method() << ", ";
				}

				ofs_ << "aquarius::" << message_field_ptr_->protocol() << "_request_header, ";

				ofs_ << message_field_ptr_->request()->name() << ">;" << std::endl;
			}

			void message_field_generator::generate_proto_response_define()
			{
				ofs_ << "using " << message_field_ptr_->name() << "_" << message_field_ptr_->protocol()
					 << "_response = aquarius::virgo::" << message_field_ptr_->protocol() << "_response<";

				if (message_field_ptr_->protocol() == "http")
				{
					ofs_ << "aquarius::virgo::http_method::" << message_field_ptr_->method() << ", ";
				}

				ofs_ << "aquarius::" << message_field_ptr_->protocol() << "_response_header, ";

				ofs_ << message_field_ptr_->response()->name() << ">;" << std::endl;
			}


			void message_field_generator::generate_construction_src(std::shared_ptr<data_field> data_ptr,
																	bool has_member)
			{
				if (!has_member || !data_ptr)
					return;

				auto name = data_ptr->name();

				ofs_ << name << "::" << name << "()" << std::endl;

				auto sp = "\t: ";

				bool start = false;

				for (auto& s : data_ptr->fields())
				{
					ofs_ << sp << s.second <<"()" << std::endl;

					if (!start)
					{
						sp = "\t, ";
						start = !start;
					}
				}

				ofs_ << "{}" << std::endl;
			}

			void message_field_generator::generate_serialize_method_src(std::shared_ptr<data_field> data_ptr, bool has_request)
			{
				ofs_ << std::endl;

				auto f = [&](const std::string& method, const std::string& direction,
							 std::shared_ptr<data_field> data_ptr, bool has_type = false)
				{
					auto m = method;

					if (m == "get" && has_request)
					{
						m = "post";
					}

					if (m == "get")
					{
						bool start = true;
						for (auto& [type, name] : data_ptr->fields())
						{
							if (!has_type)
							{
								if (start)
								{
									start = !start;
									ofs_ << "\tbuffer.sputc('?');" << std::endl;
								}
								else
									ofs_ << "\tbuffer.sputc('&');" << std::endl;
							}

							if (has_type)
							{
								ofs_ << "\t" << name << " = this->parse_" << direction << "<" << type << ">(buffer, \""
									 << name << "\");" << std::endl;
							}
							else
							{
								ofs_ << "\tthis->parse_" << direction << "(" << name << ", buffer, \"" << name << "\");"
									 << std::endl;
							}
						}
					}
					else if (m == "post")
					{
						if (has_type)
						{
							ofs_ << "\t*this = this->parse_" << direction << "<" << data_ptr->name() << ">(buffer); "
								 << std::endl;
						}
						else
						{
							ofs_ << "\tthis->parse_" << direction << "(*this, buffer);" << std::endl;
						}
					}
					else
					{
						for (auto& [type, name] : data_ptr->fields())
						{
							if (has_type)
							{
								ofs_ << "\t" << name << " = this->parse_" << direction << "<" << type << ">(buffer);"
									 << std::endl;
							}
							else
							{
								ofs_ << "\tthis->parse_" << direction << "(" << name << ", buffer);" << std::endl;
							}
						}
					}
				};

				ofs_ << "void " << data_ptr->name() << "::serialize(aquarius::flex_buffer& buffer)" << std::endl;
				ofs_ << "{" << std::endl;

				f(message_field_ptr_->method(), "to", data_ptr);

				ofs_ << "}" << std::endl;

				ofs_ << std::endl;
				ofs_ << "void " << data_ptr->name() << "::deserialize(aquarius::flex_buffer& buffer)" << std::endl;
				ofs_ << "{" << std::endl;

				f(message_field_ptr_->method(), "from", data_ptr, true);

				ofs_ << "}" << std::endl;
			}

			void message_field_generator::generate_inheritance_serialize_define(const std::string& method, bool has_response)
			{
				auto m = method;

				if (m == "get" && has_response)
				{
					m = "post";
				}

				if (m == "post")
				{
					ofs_ << " : public " << json_serialize_inheritance << std::endl;
				}
				else if (m == "get")
				{
					ofs_ << " : public " << kv_serialize_inheritance << std::endl;
				}
				else
				{
					ofs_ << " : public " << binary_serialize_inheritance << std::endl;
				}
			}

			void message_field_generator::generate_serialize_method_define()
			{
				ofs_ << "\tvirtual void serialize(aquarius::flex_buffer& buffer) override;" << std::endl;
				ofs_ << std::endl;
				ofs_ << "\tvirtual void deserialize(aquarius::flex_buffer& buffer) override;" << std::endl;
				ofs_ << std::endl;
			}

			void message_field_generator::generate_construction_define(const std::string& name, bool has_member)
			{
				ofs_ << "\t" << name << "()";

				if (!has_member)
				{
					ofs_ << " = default;";
				}
				else
				{
					ofs_ << ";";
				}

				ofs_ << std::endl;

				ofs_ << "\t" << "virtual ~" << name << "() = default;" << std::endl;

				ofs_ << std::endl;
			}
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius