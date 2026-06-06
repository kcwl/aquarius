#include "cpp_generator.h"

namespace aquarius
{
	namespace lazytool
	{
		bool cpp_generator::run(std::fstream& header, std::fstream& source,
								const std::vector<std::shared_ptr<field_base>>& fields, const std::string& protocol)
		{
			bool result = true;

			for (const auto& field : fields)
			{
				header << "\n";
				source << "\n";

				if (field->type() == struct_type::message)
				{
					result =
						generate_message(header, source, std::dynamic_pointer_cast<message_field>(field), protocol);
				}
				else if (field->type() == struct_type::structure)
				{
					result = generate_normal_data(header, source, field);
				}
				else if (field->type() == struct_type::enumture)
				{
					result = generate_normal_data(header, source, field);
				}
			}

			header << "\n";

			if (protocol == "http")
			{
				for (const auto& field : fields)
				{
					if (field->type() == struct_type::message)
					{
						auto field_ptr = std::dynamic_pointer_cast<message_field>(field);

						auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
						{
							((generate_from_tag(source, field_ptr->get<I>(), true),
							  generate_to_tag(source, field_ptr->get<I>(), true)),
							 ...);
							;
						};

						f(std::make_index_sequence<message_field::message_part>());
					}
					else if (field->type() == struct_type::structure || field->type() == struct_type::enumture)
					{
						generate_json_from_define(header, field->name());
						generate_json_to_define(header, field->name());

						generate_from_tag(source, field);
						generate_to_tag(source, field);
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

					generate_protocol_alias_define(header, message_field_ptr, protocol);
				}
			}

			return result;
		}

		bool cpp_generator::generate_message(std::fstream& header, std::fstream& source,
											 std::shared_ptr<message_field> field_ptr, const std::string& protocol)
		{
			auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
			{
				(generate_data_field_define(header, field_ptr->get<I>(), protocol), ...);
				(genetate_source(source, field_ptr->get<I>(), protocol), ...);
			};

			f(std::make_index_sequence<message_field::message_part>{});

			return true;
		}
		bool cpp_generator::generate_normal_data(std::fstream& header, std::fstream&,
												 std::shared_ptr<field_base> field_ptr, const std::string& suffix)
		{
			generate_header(header, field_ptr, suffix);
			header << std::endl << "{" << std::endl;

			// if (field_ptr->type() != struct_type::enumture)
			//{
			//   generate_equal_define(header, field_ptr);
			//   header << std::endl;
			//   generate_equal_src(source, field_ptr);
			// }

			char end = ';';
			if (field_ptr->type() == struct_type::enumture)
				end = ',';

			generate_member_variable_define(header, field_ptr, end);

			header << "};\n";

			return true;
		}

		bool cpp_generator::generate_data_field_define(std::fstream& ofs, std::shared_ptr<field_base> field_ptr,
													   const std::string& protocol)
		{
			generate_header(ofs, field_ptr);

			generate_inheritance_serialize_define(ofs, protocol);

			ofs << "{" << std::endl;

			scope_public(ofs);
			generate_construction_define(ofs, field_ptr);

			// scope_public(ofs);
			// generate_equal_define(ofs, field_ptr);

			if (protocol == "http")
			{
				scope_private(ofs);
				ofs << "\tfriend ";
				generate_json_from_define(ofs, field_ptr->name());
				ofs << "\tfriend ";
				generate_json_to_define(ofs, field_ptr->name());
			}

			ofs << std::endl;

			scope_public(ofs);
			generate_serialize_method_define(ofs);

			ofs << std::endl;
			generate_member_func_define(ofs, field_ptr);

			scope_private(ofs);
			generate_impl_member_variable_define(ofs, field_ptr);

			ofs << "};\n";

			return true;
		}

		bool cpp_generator::generate_header(std::fstream& ofs, std::shared_ptr<field_base> field_ptr,
											const std::string& suffix)
		{
			if (!suffix.empty())
			{
				ofs << "struct " << field_ptr->name() << suffix;

				return true;
			}

			switch (field_ptr->type())
			{
			case struct_type::structure:
				ofs << "struct ";
				break;
			case struct_type::enumture:
				ofs << "enum ";
				break;
			case struct_type::request:
			case struct_type::response:
			case struct_type::message:
				ofs << "class ";
				break;
			default:
				return false;
			}

			ofs << field_ptr->name() << suffix;

			return true;
		}

		void cpp_generator::generate_inheritance_serialize_define(std::fstream& ofs, const std::string& protocol)
		{
			constexpr static auto http_serialize_inheritance = "aquarius::http_serialize"sv;
			constexpr static auto tcp_serialize_inheritance = "aquarius::tcp_serialize"sv;

			ofs << ": public ";

			if (protocol == "http")
			{
				ofs << http_serialize_inheritance;
			}
			else if (protocol == "tcp")
			{
				ofs << tcp_serialize_inheritance;
			}

			ofs << std::endl;
		}

		bool cpp_generator::generate_construction_define(std::fstream& ofs, std::shared_ptr<field_base> field_ptr)
		{
			ofs << "\t" << field_ptr->name() << "();" << std::endl;

			ofs << "\t" << "virtual ~" << field_ptr->name() << "();" << std::endl;

			ofs << std::endl;

			ofs << "\t" << field_ptr->name() << "(" << field_ptr->name() << "&&) = default;" << std::endl;
			ofs << "\t" << field_ptr->name() << "& operator=(" << field_ptr->name() << "&&) = default;" << std::endl;

			return true;
		}

		// bool cpp_generator::generate_equal_define(std::fstream& ofs, std::shared_ptr<data_field> field_ptr)
		//{
		//   ofs << "\tbool operator==(const " << field_ptr->name() << " & other) const; " << std::endl;

		//  return true;
		//}

		bool cpp_generator::generate_serialize_method_define(std::fstream& ofs)
		{
			ofs << "\tvirtual void serialize(aquarius::flex_buffer& buffer) override;" << std::endl;
			ofs << std::endl;
			ofs << "\tvirtual void deserialize(aquarius::flex_buffer& buffer) override;" << std::endl;
			ofs << std::endl;

			return true;
		}

		bool cpp_generator::generate_member_func_define(std::fstream& ofs, std::shared_ptr<field_base> field_ptr)
		{
			if (field_ptr->fields().empty())
			{
				return false;
			}

			for (auto& f : field_ptr->fields())
			{
				ofs << "\t" << f.first << " " << f.second << "() const;" << std::endl;
				ofs << "\t" << f.first << "& " << f.second << "();" << std::endl;
				ofs << std::endl;
			}

			return true;
		}

		void cpp_generator::generate_impl_member_variable_define(std::fstream& ofs,
																 std::shared_ptr<field_base>)
		{
			ofs << "\tstruct impl;" << std::endl;
			ofs << "\tstd::shared_ptr<impl> impl_ptr_;" << std::endl;
		}

		bool cpp_generator::generate_member_variable_define(std::fstream& ofs, std::shared_ptr<field_base> field_ptr,
															char end)
		{
			for (auto& [type, name] : field_ptr->fields())
			{
				ofs << "\t" << type << " " << name << end << std::endl;
			}

			return true;
		}

		bool cpp_generator::generate_protocol_alias_define(std::fstream& ofs, std::shared_ptr<message_field> field_ptr,
														   const std::string& protocol)
		{
			generate_request_alias_define(ofs, field_ptr->get<message_field::tag_request>(), field_ptr->name(),
										  protocol, field_ptr->router());
			generate_response_alias_define(ofs, field_ptr->get<message_field::tag_response>(), field_ptr->name(),
										   protocol, field_ptr->router());

			return true;
		}

		bool cpp_generator::generate_request_alias_define(std::fstream& ofs, std::shared_ptr<field_base> field_ptr,
														  const std::string& message_name, const std::string protocol,
														  const std::string& router)
		{
			ofs << "using " << message_name << "_request = aquarius::" << protocol << "_request<\""
				<< router << "\", ";

			ofs << field_ptr->name() << ">;" << std::endl;

			return true;
		}

		bool cpp_generator::generate_response_alias_define(std::fstream& ofs, std::shared_ptr<field_base> field_ptr,
														   const std::string& message_name, const std::string protocol, const std::string& router)
		{
			ofs << "using " << message_name << "_response = aquarius::" << protocol << "_response<";

			if (protocol == "tcp")
			{
				ofs << "\"" << router << "\", ";
			}

			ofs << field_ptr->name() << ">;" << std::endl;

			return true;
		}

		bool cpp_generator::scope_public(std::fstream& ofs)
		{
			ofs << "public:" << std::endl;

			return true;
		}

		bool cpp_generator::scope_private(std::fstream& ofs)
		{
			ofs << "private:" << std::endl;

			return true;
		}

		bool cpp_generator::genetate_source(std::fstream& ofs, std::shared_ptr<field_base> field_ptr, const std::string& protocol)
		{
			generate_normal_data(ofs, ofs, field_ptr, "::impl");

			generate_construction_src(ofs, field_ptr);

			// generate_equal_src(ofs, field_ptr);

			generate_serialize_method_src(ofs, field_ptr, protocol);

			generate_member_func_src(ofs, field_ptr);

			return true;
		}

		bool cpp_generator::generate_construction_src(std::fstream& ofs, std::shared_ptr<field_base> field_ptr)
		{
			auto name = field_ptr->name();

			ofs << name << "::" << name << "()" << std::endl;

			ofs << "\t: impl_ptr_(std::make_shared<impl>())" << std::endl;

			ofs << "{}" << std::endl;

			ofs << std::endl;

			ofs << name << "::~" << name << "(){}" << std::endl;

			return true;
		}

		// bool cpp_generator::generate_equal_src(std::fstream& ofs, std::shared_ptr<data_field> field_ptr)
		//{
		//   ofs << std::endl;

		//  ofs << "bool " << field_ptr->name() << "::operator==(const " << field_ptr->name() << "& other) const"
		//      << std::endl;
		//  ofs << "{" << std::endl;

		//  bool start = false;

		//  auto start_sp = "";

		//  ofs << "\treturn ";

		//  if (field_ptr->fields().empty())
		//  {
		//      ofs << "true";
		//  }
		//  else
		//  {
		//      for (auto& s : field_ptr->fields())
		//      {
		//          ofs << start_sp << s.second << " == other." << s.second;

		//          if (!start)
		//          {
		//              start_sp = " && ";
		//              start = !start;
		//          }
		//      }
		//  }

		//  ofs << ";" << std::endl;
		//  ofs << "}" << std::endl;

		//  return true;
		//}

		bool cpp_generator::generate_serialize_method_src(std::fstream& ofs, std::shared_ptr<field_base> field_ptr, const std::string& protocol)
		{
			ofs << std::endl;

			auto tcp_f = [&](const std::string& direction, std::shared_ptr<field_base> data_ptr, bool has_type = false)
			{
				if (has_type)
				{
					ofs << "\t*this->impl_ptr_ = this->parse_" << direction << "<" << data_ptr->name() <<"::impl"
						<< ">(buffer); " << std::endl;
				}
				else
				{
					ofs << "\tthis->parse_" << direction << "(*this->impl_ptr_, buffer);" << std::endl;
				}
			};

			auto http_f = [&] (const std::string& direction, std::shared_ptr<field_base> data_ptr, bool has_type = false)
				{
					if (!has_type)
					{
						ofs << "\tif(this->method() == http_method::post)" << std::endl;
						ofs << "\t{" << std::endl;
						ofs <<"\t\tjson_base::parse_." << direction<< "_datas<" << data_ptr->name() << ">(*this, buffer); " << std::endl;
						ofs << "\t}" << std::endl;
						ofs << "\telse" << std::endl;
						ofs << "\t{" << std::endl;
						
						bool first = true;

						for (auto& m : field_ptr->fields())
						{
							if (first)
							{
								ofs << "\t\tbuffer.sputc('?');";
								first = false;
							}
							else
							{
								ofs << "\t\tbuffer.sputc('&');";
							}

							ofs << std::endl;
							ofs << "\t\tkv_base::parse_" << direction << "(impl_ptr_->" << m.second << ", buffer,\"" << m.second << "\");" << std::endl;
						}

						ofs << "\t}" << std::endl;
					}
					else
					{
						ofs << "\tif(this->method() == http_method::post)" << std::endl;
						ofs << "\t{" << std::endl;
						ofs << "\t\t*this = json_base::parse_." << direction << "_datas<" << data_ptr->name() << ">(buffer); " << std::endl;
						ofs << "\t}" << std::endl;
						ofs << "\telse" << std::endl;
						ofs << "\t{" << std::endl;

						for (auto& m : field_ptr->fields())
						{
							ofs << "\t\timpl_ptr_->"<<m.second << " = kv_base::parse_" << direction << "<"<< m.first << ">(buffer, \""<< m.second << "\");" << std::endl;
						}

						ofs << "\t}" << std::endl;
					}
				};

			ofs << "void " << field_ptr->name() << "::serialize(aquarius::flex_buffer& buffer)" << std::endl;
			ofs << "{" << std::endl;

			protocol == "tcp" ? tcp_f("to", field_ptr):http_f("to", field_ptr);

			ofs << "}" << std::endl;

			ofs << std::endl;
			ofs << "void " << field_ptr->name() << "::deserialize(aquarius::flex_buffer& buffer)" << std::endl;
			ofs << "{" << std::endl;

			protocol == "tcp" ? tcp_f("from", field_ptr, true) : http_f("from", field_ptr, true);

			ofs << "}" << std::endl;

			return true;
		}

		bool cpp_generator::generate_member_func_src(std::fstream& ofs, std::shared_ptr<field_base> field_ptr)
		{
			if (field_ptr->fields().empty())
			{
				return false;
			}

			for (auto& f : field_ptr->fields())
			{
				ofs << f.first << " " << field_ptr->name() << "::" << f.second << "() const" << std::endl;
				ofs << "{" << std::endl;
				ofs << "\treturn impl_ptr_->" << f.second << ";" << std::endl;
				ofs << "}";
				ofs << std::endl;
				ofs << f.first << "& " << field_ptr->name() << "::" << f.second << "()" << std::endl;
				ofs << "{" << std::endl;
				ofs << "\treturn impl_ptr_->" << f.second << ";" << std::endl;
				ofs << "}";
				ofs << std::endl;
			}

			return true;
		}

		void cpp_generator::generate_to_tag(std::fstream& ofs, std::shared_ptr<field_base> parser, bool has_impl)
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
				if (generate_to_int(ofs, key, value, has_impl))
					continue;

				if (generate_to_string(ofs, key, value, has_impl))
					continue;

				if (generate_to_array(ofs, key, value, has_impl))
					continue;

				generate_to_object(ofs, key, value, has_impl);
			}

			ofs << "\treturn result;" << std::endl;

			ofs << "}" << std::endl;
		}

		void cpp_generator::generate_from_tag(std::fstream& ofs, std::shared_ptr<field_base> parser, bool has_impl)
		{
			ofs << std::endl;
			ofs << "void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const "
				<< parser->name() << "& local)" << std::endl;
			ofs << "{" << std::endl;
			ofs << "\tauto& jv_obj = jv.emplace_object();" << std::endl;

			for (auto& [key, value] : parser->fields())
			{
				if (generate_from_int(ofs, key, value, has_impl))
					continue;

				if (generate_from_string(ofs, key, value, has_impl))
					continue;

				if (generate_from_array(ofs, key, value, has_impl))
					continue;

				generate_from_object(ofs, key, value, has_impl);
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

		bool cpp_generator::generate_to_int(std::fstream& ofs, const std::string& type, const std::string& value,
											bool has_impl)
		{
			auto ty = check_type(type);

			if (ty != json_type::integer)
				return false;

			if (type == "int32" || type == "uint32" || type == "float" || type == "uint64")
			{
				ofs << "\tresult.";

				if (has_impl)
				{
					ofs << "impl_ptr_->";
				}

				ofs << value << " = static_cast<" << type << ">(obj->at(\"" << value << "\").as_";

				if (type == "int32" || type == "uint32" || type == "uint64")
					ofs << "int64";
				else if (type == "float")
					ofs << "double";

				ofs << "());" << std::endl;
			}
			else if (type == "int64" || type == "double" || type == "bool")
			{
				ofs << "\tresult.";

				if (has_impl)
				{
					ofs << "impl_ptr_->";
				}

				ofs << value << " = obj->at(\"" << value << "\").as_" << type << "();" << std::endl;
			}

			return true;
		}

		bool cpp_generator::generate_to_string(std::fstream& ofs, const std::string& type, const std::string& value,
											   bool has_impl)
		{
			auto ty = check_type(type);

			if (ty != json_type::string)
				return false;

			ofs << "\tresult.";

			if (has_impl)
			{
				ofs << "impl_ptr_->";
			}

			ofs << value << " = static_cast<" << type << ">(obj->at(\"" << value << "\").as_string());" << std::endl;

			return true;
		}

		bool cpp_generator::generate_to_array(std::fstream& ofs, const std::string& type, const std::string& value,
											  bool has_impl)
		{
			auto ty = check_type(type);

			if (ty != json_type::array)
				return false;

			ofs << "\tresult.";

			if (has_impl)
			{
				ofs << "impl_ptr_->";
			}

			ofs << value << " = aquarius::json_value_to_array(obj->at(\"" << value << "\"));" << std::endl;

			return true;
		}

		bool cpp_generator::generate_to_object(std::fstream& ofs, const std::string& type, const std::string& value,
											   bool has_impl)
		{
			auto ty = check_type(type);

			if (ty != json_type::object)
				return false;

			ofs << "\tresult.";

			if (has_impl)
			{
				ofs << "impl_ptr_->";
			}

			ofs << value << " = aquarius::json::value_to<" << type << ">(obj->at(\"" << value << "\"));" << std::endl;

			return true;
		}

		bool cpp_generator::generate_from_int(std::fstream& ofs, const std::string& type, const std::string& value,
											  bool has_impl)
		{
			auto ty = check_type(type);

			if (ty != json_type::integer)
				return false;

			ofs << "\tjv_obj.emplace(\"" << value << "\", local.";

			if (has_impl)
			{
				ofs << "impl_ptr_->";
			}

			ofs << value << ");" << std::endl;

			return true;
		}

		bool cpp_generator::generate_from_string(std::fstream& ofs, const std::string& type, const std::string& value,
												 bool has_impl)
		{
			auto ty = check_type(type);

			if (ty != json_type::string)
				return false;

			ofs << "\tjv_obj.emplace(\"" << value << "\", local.";

			if (has_impl)
			{
				ofs << "impl_ptr_->";
			}

			ofs << value << ");" << std::endl;

			return true;
		}

		bool cpp_generator::generate_from_array(std::fstream& ofs, const std::string& type, const std::string& value,
												bool has_impl)
		{
			auto ty = check_type(type);

			if (ty != json_type::array)
				return false;

			ofs << "\tjv_obj.emplace(\"" << value << "\", aquarius::json_value_from_array(local.";

			if (has_impl)
			{
				ofs << "impl_ptr_->";
			}

			ofs << value << "));" << std::endl;

			return true;
		}

		bool cpp_generator::generate_from_object(std::fstream& ofs, const std::string& type, const std::string& value,
												 bool has_impl)
		{
			auto ty = check_type(type);

			if (ty != json_type::object)
				return false;

			ofs << "\tjv_obj.emplace(\"" << value << "\", aquarius::json_value_from_object<" << type << ">(local.";

			if (has_impl)
			{
				ofs << "impl_ptr_->";
			}

			ofs << value << "));" << std::endl;

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

		bool cpp_generator::generate_model(std::fstream& ofs, std::shared_ptr<field_base> field_ptr)
		{
			ofs << "struct " << field_ptr->name() << std::endl;
			ofs << "{" << std::endl;

			auto data_field_ptr = std::dynamic_pointer_cast<field_base>(field_ptr);

			generate_model_field(ofs, data_field_ptr);

			generate_model_member_func(ofs, data_field_ptr);

			generate_model_member_name_func(ofs, data_field_ptr);

			ofs << "};";

			return true;
		}

		void cpp_generator::generate_model_field(std::fstream& ofs, std::shared_ptr<field_base> model_field_ptr)
		{
			for (auto& field : model_field_ptr->fields())
			{
				if (field.first.empty())
					continue;

				ofs << "\taquarius::tbl::fields<" << field.first << "> ";

				ofs << field.second << ";" << std::endl;
			}
		}

		void cpp_generator::generate_model_member_func(std::fstream& ofs, std::shared_ptr<field_base> model_field_ptr)
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
															std::shared_ptr<field_base> model_field_ptr)
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