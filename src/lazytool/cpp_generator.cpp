#include "cpp_generator.h"

namespace aquarius
{
	namespace lazytool
	{
		bool cpp_generator::run(std::fstream& header, std::fstream& source,
								const std::vector<std::shared_ptr<field>>& fields, const std::string& protocol)
		{
			bool result = true;

			for (const auto& field : fields)
			{
				header << "\n";
				source << "\n";

				if (field->type() == struct_type::message)
				{
					result = generate_message(header, source, std::dynamic_pointer_cast<message_field>(field));
				}
				else if (field->type() == struct_type::structure)
				{
					result = generate_normal_data(header, source, std::dynamic_pointer_cast<data_field>(field));
				}
				else if (field->type() == struct_type::enumture)
				{
					result = generate_normal_data(header, source, std::dynamic_pointer_cast<data_field>(field));
				}
			}

			header << "\n";

			if (protocol == "http")
			{
				for (auto& name : json_generator_)
				{
					generate_json_from_define(header, name);
					generate_json_to_define(header, name);

					auto iter = std::find_if(fields.begin(), fields.end(),
											 [&] (std::shared_ptr<field> field_ptr) { return name == field_ptr->name(); });

					if (iter == fields.end())
					{
						continue;
					}

					generate_from_tag(source, std::dynamic_pointer_cast<data_field>(*iter));
					generate_to_tag(source, std::dynamic_pointer_cast<data_field>(*iter));
				}

				for (const auto& field : fields)
				{
					if (field->type() == struct_type::message)
					{
						auto field_ptr = std::dynamic_pointer_cast<message_field>(field);

						generate_json_from_define(header, field_ptr->request()->name());
						generate_json_to_define(header, field_ptr->response()->name());

						if (field_ptr->method() != "get")
						{
							generate_from_tag(source, field_ptr->request());
							generate_to_tag(source, field_ptr->request());
						}

						generate_from_tag(source, field_ptr->response());
						generate_to_tag(source, field_ptr->response());
					}
				}
			}
			
			header << "\n";

			for (const auto& field : fields)
			{
				if (field->type() == struct_type::message)
				{
					auto message_field_ptr = std::dynamic_pointer_cast<message_field>(field);
					if (!message_field_ptr)
					{
						continue;
					}

					generate_protocol_alias_define(header, message_field_ptr);
				}
			}

			return result;
		}

		bool cpp_generator::generate_message(std::fstream& header, std::fstream& source,
											 std::shared_ptr<message_field> field_ptr)
		{
			generate_data_field_define(header, field_ptr->request(), field_ptr->method());
			generate_data_field_define(header, field_ptr->response(), field_ptr->method(), true);

			genetate_source(source, field_ptr->request(), field_ptr->method());
			genetate_source(source, field_ptr->response(), field_ptr->method(), true);

			return true;
		}
		bool cpp_generator::generate_normal_data(std::fstream& header, std::fstream& source,
												 std::shared_ptr<data_field> field_ptr)
		{
			generate_header(header, field_ptr);
			header << std::endl << "{" << std::endl;

			if (field_ptr->type() == struct_type::structure)
			{
				generate_equal_define(header, field_ptr);
			}

			header << std::endl;

			char end = ';';
			if (field_ptr->type() == struct_type::enumture)
				end = ',';

			generate_member_variable_define(header, field_ptr, end);

			header << "};\n";

			generate_equal_src(source, field_ptr);

			return true;
		}

		bool cpp_generator::generate_data_field_define(std::fstream& ofs, std::shared_ptr<data_field> field_ptr,
													   const std::string& method, bool has_response)
		{
			generate_header(ofs, field_ptr);

			bool has_json = false;
			generate_inheritance_serialize_define(ofs, method, has_json, has_response);

			ofs << "{" << std::endl;

			scope_public(ofs);
			generate_construction_define(ofs, field_ptr);

			scope_public(ofs);
			generate_equal_define(ofs, field_ptr);

			ofs << std::endl;

			scope_public(ofs);
			generate_serialize_method_define(ofs);

			scope_public(ofs);
			generate_member_variable_define(ofs, field_ptr, ';');

			ofs << "};\n";

			return true;
		}

		bool cpp_generator::generate_header(std::fstream& ofs, std::shared_ptr<data_field> field_ptr)
		{
			switch (field_ptr->type())
			{
			case struct_type::structure:
				ofs << "struct ";
				break;
			case struct_type::enumture:
				ofs << "enum ";
				break;
			case struct_type::message:
				ofs << "class ";
				break;
			default:
				return false;
			}

			ofs << field_ptr->name();

			return true;
		}

		bool cpp_generator::generate_inheritance_serialize_define(std::fstream& ofs, const std::string& method,
																  bool& has_json, bool has_response)
		{
			constexpr static auto json_serialize_inheritance = "aquarius::http_json_serialize"sv;
			constexpr static auto kv_serialize_inheritance = "aquarius::http_kv_serialize"sv;
			constexpr static auto binary_serialize_inheritance = "aquarius::tcp_binary_serialize"sv;

			auto m = method;

			if (m == "get" && has_response)
			{
				m = "post";
			}

			if (m == "post")
			{
				ofs << " : public " << json_serialize_inheritance << std::endl;
				has_json = true;
			}
			else if (m == "get")
			{
				ofs << " : public " << kv_serialize_inheritance << std::endl;
			}
			else
			{
				ofs << " : public " << binary_serialize_inheritance << std::endl;
			}

			return true;
		}

		bool cpp_generator::generate_construction_define(std::fstream& ofs, std::shared_ptr<data_field> field_ptr)
		{
			ofs << "\t" << field_ptr->name() << "()";

			if (field_ptr->fields().empty())
			{
				ofs << " = default;";
			}
			else
			{
				ofs << ";";
			}

			ofs << std::endl;

			ofs << "\t" << "virtual ~" << field_ptr->name() << "() = default;" << std::endl;

			ofs << std::endl;

			return true;
		}

		bool cpp_generator::generate_equal_define(std::fstream& ofs, std::shared_ptr<data_field> field_ptr)
		{
			ofs << "\tbool operator==(const " << field_ptr->name() << " & other) const; " << std::endl;

			return true;
		}

		bool cpp_generator::generate_serialize_method_define(std::fstream& ofs)
		{
			ofs << "\tvirtual void serialize(aquarius::flex_buffer& buffer) override;" << std::endl;
			ofs << std::endl;
			ofs << "\tvirtual void deserialize(aquarius::flex_buffer& buffer) override;" << std::endl;
			ofs << std::endl;

			return true;
		}

		bool cpp_generator::generate_member_variable_define(std::fstream& ofs, std::shared_ptr<data_field> field_ptr,
															char end)
		{
			for (auto& [type, name] : field_ptr->fields())
			{
				ofs << "\t" << type << " " << name << end << std::endl;

				if (check_type(type) == json_type::object)
				{
					json_generator_.insert(type);
				}
			}

			return true;
		}

		bool cpp_generator::generate_protocol_alias_define(std::fstream& ofs, std::shared_ptr<message_field> field_ptr)
		{
			generate_request_alias_define(ofs, field_ptr->request(),field_ptr->name(), field_ptr->protocol(), field_ptr->router(),
										  field_ptr->method());
			generate_response_alias_define(ofs, field_ptr->response(), field_ptr->name(), field_ptr->protocol(), field_ptr->method());

			return true;
		}

		bool cpp_generator::generate_request_alias_define(std::fstream& ofs, std::shared_ptr<data_field> field_ptr, const std::string& message_name,
														  const std::string protocol, const std::string& router,
														  const std::string& method)
		{
			ofs << "using " << message_name << "_request = aquarius::virgo::" << protocol << "_request<\""
				<< router << "\", ";

			if (protocol == "http")
			{
				ofs << "aquarius::virgo::http_method::" << method << ", ";
			}

			ofs << field_ptr->name() << ">;" << std::endl;

			return true;
		}

		bool cpp_generator::generate_response_alias_define(std::fstream& ofs, std::shared_ptr<data_field> field_ptr, const std::string& message_name,
														   const std::string protocol, const std::string& method)
		{
			ofs << "using " << message_name << "_response = aquarius::virgo::" << protocol << "_response<";

			if (protocol == "http")
			{
				ofs << "aquarius::virgo::http_method::" << method << ", ";
			}

			ofs << field_ptr->name() << ">;" << std::endl;

			return true;
		}

		bool cpp_generator::scope_public(std::fstream& ofs)
		{
			ofs << "public:" << std::endl;

			return true;
		}

		bool cpp_generator::genetate_source(std::fstream& ofs, std::shared_ptr<data_field> field_ptr,
											const std::string& method, bool has_response)
		{
			generate_construction_src(ofs, field_ptr);

			generate_equal_src(ofs, field_ptr);

			generate_serialize_method_src(ofs, field_ptr, method, has_response);

			return true;
		}

		bool cpp_generator::generate_construction_src(std::fstream& ofs, std::shared_ptr<data_field> field_ptr)
		{
			if (field_ptr->fields().empty() || !field_ptr)
				return false;

			auto name = field_ptr->name();

			ofs << name << "::" << name << "()" << std::endl;

			auto sp = "\t: ";

			bool start = false;

			for (auto& s : field_ptr->fields())
			{
				ofs << sp << s.second << "()" << std::endl;

				if (!start)
				{
					sp = "\t, ";
					start = !start;
				}
			}

			ofs << "{}" << std::endl;

			return true;
		}

		bool cpp_generator::generate_equal_src(std::fstream& ofs, std::shared_ptr<data_field> field_ptr)
		{
			ofs << std::endl;

			ofs << "bool " << field_ptr->name() << "::operator==(const " << field_ptr->name() << "& other) const"
				<< std::endl;
			ofs << "{" << std::endl;

			bool start = false;

			auto start_sp = "";

			ofs << "\treturn ";

			if (field_ptr->fields().empty())
			{
				ofs << "true";
			}
			else
			{
				for (auto& s : field_ptr->fields())
				{
					ofs << start_sp << s.second << " == other." << s.second;

					if (!start)
					{
						start_sp = " && ";
						start = !start;
					}
				}
			}

			ofs << ";" << std::endl;
			ofs << "}" << std::endl;

			return true;
		}

		bool cpp_generator::generate_serialize_method_src(std::fstream& ofs, std::shared_ptr<data_field> field_ptr,
														  const std::string& method, bool has_response)
		{
			ofs << std::endl;

			auto f = [&](const std::string& direction, std::shared_ptr<data_field> data_ptr, bool has_type = false)
			{
				auto m = method;

				if (m == "get" && has_response)
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
								ofs << "\tbuffer.sputc('?');" << std::endl;
							}
							else
								ofs << "\tbuffer.sputc('&');" << std::endl;
						}

						if (has_type)
						{
							ofs << "\t" << name << " = this->parse_" << direction << "<" << type << ">(buffer, \""
								<< name << "\");" << std::endl;
						}
						else
						{
							ofs << "\tthis->parse_" << direction << "(" << name << ", buffer, \"" << name << "\");"
								<< std::endl;
						}
					}
				}
				else if (m == "post")
				{
					if (has_type)
					{
						ofs << "\t*this = this->parse_" << direction << "<" << data_ptr->name() << ">(buffer); "
							<< std::endl;
					}
					else
					{
						ofs << "\tthis->parse_" << direction << "(*this, buffer);" << std::endl;
					}
				}
				else
				{
					for (auto& [type, name] : data_ptr->fields())
					{
						if (has_type)
						{
							ofs << "\t" << name << " = this->parse_" << direction << "<" << type << ">(buffer);"
								<< std::endl;
						}
						else
						{
							ofs << "\tthis->parse_" << direction << "(" << name << ", buffer);" << std::endl;
						}
					}
				}
			};

			ofs << "void " << field_ptr->name() << "::serialize(aquarius::flex_buffer& buffer)" << std::endl;
			ofs << "{" << std::endl;

			f("to", field_ptr);

			ofs << "}" << std::endl;

			ofs << std::endl;
			ofs << "void " << field_ptr->name() << "::deserialize(aquarius::flex_buffer& buffer)" << std::endl;
			ofs << "{" << std::endl;

			f("from", field_ptr, true);

			ofs << "}" << std::endl;

			return true;
		}
		void cpp_generator::generate_to_tag(std::fstream& ofs, std::shared_ptr<data_field> parser)
		{
			ofs << std::endl;

			ofs << parser->name() << " tag_invoke(const aquarius::json::value_to_tag<" << parser->name()
				<< ">&, const aquarius::json::value& jv)" << std::endl;
			ofs << "{" << std::endl;
			ofs << "\t" << parser->name() << " result{};" << std::endl;
			ofs << "\tauto obj = jv.try_as_object();" << std::endl;
			ofs << "\tif(obj->empty())" << std::endl;
			ofs << "\t\treturn {};" << std::endl;

			for (auto& [key, value] : parser->fields())
			{
				if (generate_to_int(ofs, key, value))
					continue;

				if (generate_to_string(ofs, key, value))
					continue;

				if (generate_to_array(ofs, key, value))
					continue;

				generate_to_object(ofs, key, value);
			}

			ofs << "\treturn result;" << std::endl;

			ofs << "}" << std::endl;
		}

		void cpp_generator::generate_from_tag(std::fstream& ofs, std::shared_ptr<data_field> parser)
		{
			ofs << std::endl;
			ofs << "void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const "
				<< parser->name() << "& local)" << std::endl;
			ofs << "{" << std::endl;
			ofs << "\tauto& jv_obj = jv.emplace_object();" << std::endl;

			for (auto& [key, value] : parser->fields())
			{
				if (generate_from_int(ofs, key, value))
					continue;

				if (generate_from_string(ofs, key, value))
					continue;

				if (generate_from_array(ofs, key, value))
					continue;

				generate_from_object(ofs, key, value);
			}
			ofs << "}" << std::endl;
		}

		void cpp_generator::generate_json_from_define(std::fstream& ofs, const std::string& field_name)
		{
			ofs << "void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const "
				<< field_name << "& local);" << std::endl;
		}

		void cpp_generator::generate_json_to_define(std::fstream& ofs, const std::string& field_name)
		{
			ofs << field_name << " tag_invoke(const aquarius::json::value_to_tag<" << field_name
				<< ">&, const aquarius::json::value& jv);" << std::endl;
		}

		bool cpp_generator::generate_to_int(std::fstream& ofs, const std::string& type, const std::string& value)
		{
			auto ty = check_type(type);

			if (ty != json_type::integer)
				return false;

			if (type == "int32" || type == "uint32" || type == "float" || type == "uint64")
			{
				ofs << "\tresult." << value << " = static_cast<" << type << ">(obj->at(\"" << value << "\").as_";

				if (type == "int32" || type == "uint32" || type == "uint64")
					ofs << "int64";
				else if (type == "float")
					ofs << "double";

				ofs << "());" << std::endl;
			}
			else if (type == "int64" || type == "double" || type == "bool")
			{
				ofs << "\tresult." << value << " = obj->at(\"" << value << "\").as_" << type << "();" << std::endl;
			}

			return true;
		}

		bool cpp_generator::generate_to_string(std::fstream& ofs, const std::string& type, const std::string& value)
		{
			auto ty = check_type(type);

			if (ty != json_type::string)
				return false;

			ofs << "\tresult." << value << " = static_cast<" << type << ">(obj->at(\"" << value << "\").as_string());"
				<< std::endl;

			return true;
		}

		bool cpp_generator::generate_to_array(std::fstream& ofs, const std::string& type, const std::string& value)
		{
			auto ty = check_type(type);

			if (ty != json_type::array)
				return false;

			ofs << "\tresult." << value << " = aquarius::json_value_to_array(obj->at(\"" << value << "\"));"
				<< std::endl;

			return true;
		}

		bool cpp_generator::generate_to_object(std::fstream& ofs, const std::string& type, const std::string& value)
		{
			auto ty = check_type(type);

			if (ty != json_type::object)
				return false;

			ofs << "\tresult." << value << " = aquarius::json::value_to<" << type << ">(obj->at(\"" << value << "\"));"
				<< std::endl;

			return true;
		}

		bool cpp_generator::generate_from_int(std::fstream& ofs, const std::string& type, const std::string& value)
		{
			auto ty = check_type(type);

			if (ty != json_type::integer)
				return false;

			ofs << "\tjv_obj.emplace(\"" << value << "\", local." << value << ");" << std::endl;

			return true;
		}

		bool cpp_generator::generate_from_string(std::fstream& ofs, const std::string& type, const std::string& value)
		{
			auto ty = check_type(type);

			if (ty != json_type::string)
				return false;

			ofs << "\tjv_obj.emplace(\"" << value << "\", local." << value << ");" << std::endl;

			return true;
		}

		bool cpp_generator::generate_from_array(std::fstream& ofs, const std::string& type, const std::string& value)
		{
			auto ty = check_type(type);

			if (ty != json_type::array)
				return false;

			ofs << "\tjv_obj.emplace(\"" << value << "\", aquarius::json_value_from_array(local." << value << "));"
				<< std::endl;

			return true;
		}

		bool cpp_generator::generate_from_object(std::fstream& ofs, const std::string& type, const std::string& value)
		{
			auto ty = check_type(type);

			if (ty != json_type::object)
				return false;

			ofs << "\tjv_obj.emplace(\"" << value << "\", aquarius::json_value_from_object<" << type << ">(local."
				<< value << "));" << std::endl;

			return true;
		}

		cpp_generator::json_type cpp_generator::check_type(const std::string& type)
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

		void cpp_generator::generate_model(std::fstream& ofs, std::shared_ptr<field> field_ptr)
		{
			ofs << "struct " << field_ptr->name() << std::endl;
			ofs << "{" << std::endl;

			auto data_field_ptr = std::dynamic_pointer_cast<data_field>(field_ptr);

			generate_model_field(ofs, data_field_ptr);

			generate_model_member_func(ofs, data_field_ptr);

			generate_model_member_name_func(ofs, data_field_ptr);

			ofs << "};";
		}

		void cpp_generator::generate_model_field(std::fstream& ofs, std::shared_ptr<data_field> model_field_ptr)
		{
			for (auto& field : model_field_ptr->fields())
			{
				if (field.first.empty())
					continue;

				ofs << "\taquarius::tbl::" << field.first << " ";

				ofs << field.second << ";" << std::endl;
			}
		}

		void cpp_generator::generate_model_member_func(std::fstream& ofs, std::shared_ptr<data_field> model_field_ptr)
		{
			ofs << "\tconstexpr static auto member()\n";
			ofs << "\t{\n";
			ofs << "\t\treturn std::make_tuple(";

			for (auto& field : model_field_ptr->fields())
			{
				if (field.first.empty())
					continue;

				ofs << "\n\t\t\t&" << model_field_ptr->name() << "::" << field.second << ",";
			}

			if (!model_field_ptr->fields().empty())
			{
				ofs.seekp(-1, std::ios_base::cur);
			}

			ofs << "\n\t\t);\n";
			ofs << "\t}\n";
		}

		void cpp_generator::generate_model_member_name_func(std::fstream& ofs,
															std::shared_ptr<data_field> model_field_ptr)
		{
			ofs << "\tconstexpr static auto member_name()\n";
			ofs << "\t{\n";
			ofs << "\t\treturn std::array{";
			for (auto& field : model_field_ptr->fields())
			{
				if (field.first.empty())
					continue;

				ofs << "\n\t\t\t\"" << field.second << "\"sv,";
			}

			if (!model_field_ptr->fields().empty())
			{
				ofs.seekp(-1, std::ios_base::cur);
			}

			ofs << "\n\t\t};\n";
			ofs << "\t}\n";
		}
	} // namespace lazytool
} // namespace aquarius