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
					result = generate_message(header, source, std::dynamic_pointer_cast<message_field>(field));
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
											 std::shared_ptr<message_field> field_ptr)
		{
			auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
			{
				(generate_data_field_define(header, field_ptr->get<I>()), ...);
				(genetate_source(source, field_ptr->get<I>()), ...);
			};

			f(std::make_index_sequence<message_field::message_part>{});

			return true;
		}
		bool cpp_generator::generate_normal_data(std::fstream& header, std::fstream&,
												 std::shared_ptr<field_base> field_ptr, const std::string& suffix)
		{
			auto f = [&header, field_ptr, suffix] ()
				{
					header << "AQUARIUS_SERIALIZE_";

					if (field_ptr->type() == struct_type::enumture)
					{
						header << "ENUM";
					}
					else/* if (field_ptr->type() == struct_type::structure)*/
					{
						header << "STRUCT";
					}

					header << "(" << field_ptr->name();
					
					if (!suffix.empty())
					{
						header <<suffix;
					}
					
					header << ", (";

					for (auto& f : field_ptr->fields())
					{
						header << f.second << ", ";
					}

					if (!field_ptr->fields().empty())
					{
						header.seekp(-2, std::ios::cur);
					}

					header << "))\n";
				};

			generate_header(header, field_ptr, suffix);
			header << std::endl << "{" << std::endl;

			char end = ';';
			if (field_ptr->type() == struct_type::enumture)
				end = ',';

			generate_member_variable_define(header, field_ptr, end);

			if (!suffix.empty())
			{
				f();
				header << "};\n";
			}
			else
			{
				header << "};\n";
				f();
			}

			return true;
		}

		bool cpp_generator::generate_data_field_define(std::fstream& ofs, std::shared_ptr<field_base> field_ptr)
		{
			generate_header(ofs, field_ptr);

			generate_inheritance_serialize_define(ofs);

			ofs << "{" << std::endl;

			scope_public(ofs);
			generate_construction_define(ofs, field_ptr);

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
			case struct_type::enumture:
				ofs << "enum class ";
				break;
			case struct_type::structure:
				ofs << "struct ";
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

		void cpp_generator::generate_inheritance_serialize_define(std::fstream& ofs)
		{
			ofs << ": public aquarius::serialization\n";
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
			auto f = [&](const std::string& func_name)
			{
				ofs << "\tvirtual void to_" << func_name << "(aquarius::flex_buffer& buffer) override;" << std::endl;
				ofs << std::endl;
				ofs << "\tvirtual void from_" << func_name << "(aquarius::flex_buffer& buffer) override;" << std::endl;
				ofs << std::endl;
			};

			f("binary");
			f("json");
			f("kv");

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

		void cpp_generator::generate_impl_member_variable_define(std::fstream& ofs, std::shared_ptr<field_base>)
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
			ofs << "using " << message_name << "_request = aquarius::" << protocol << "_request<\"" << router << "\", ";

			ofs << field_ptr->name() << ">;" << std::endl;

			return true;
		}

		bool cpp_generator::generate_response_alias_define(std::fstream& ofs, std::shared_ptr<field_base> field_ptr,
														   const std::string& message_name, const std::string protocol,
														   const std::string& router)
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

		bool cpp_generator::genetate_source(std::fstream& ofs, std::shared_ptr<field_base> field_ptr)
		{
			generate_normal_data(ofs, ofs, field_ptr, "::impl");

			generate_construction_src(ofs, field_ptr);

			generate_serialize_method_src(ofs, field_ptr);

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

		bool cpp_generator::generate_serialize_method_src(std::fstream& ofs, std::shared_ptr<field_base> field_ptr)
		{
			ofs << std::endl;

			generate_seialize_method_binary(ofs, field_ptr);
			generate_seialize_method_json(ofs, field_ptr);
			generate_seialize_method_kv(ofs, field_ptr);

			return true;
		}

		bool cpp_generator::generate_seialize_method_binary(std::fstream& ofs, std::shared_ptr<field_base> field_ptr)
		{
			auto to_func = [&]()
			{
				ofs << "void " << field_ptr->name() << "::to_binary(flex_buffer& buffer)\n";
				ofs << "{\n";
				if (!field_ptr->fields().empty())
				{
					for (auto& field : field_ptr->fields())
					{
						ofs << "\tbinary_parse::to_datas(impl_ptr_->" << field.second << ", buffer);\n";
					}
				}

				ofs << "}\n";
			};

			auto from_func = [&]()
			{
				ofs << "void " << field_ptr->name() << "::from_binary(flex_buffer& buffer)\n";
				ofs << "{\n";
				if (!field_ptr->fields().empty())
				{
					for (auto& field : field_ptr->fields())
					{
						ofs << "\timpl_ptr_->" << field.second << " = binary_parse::from_datas<" << field.first
							<< ">(buffer);\n";
					}
				}

				ofs << "}\n";
			};

			to_func();

			from_func();

			return true;
		}

		bool cpp_generator::generate_seialize_method_json(std::fstream& ofs, std::shared_ptr<field_base> field_ptr)
		{
			auto to_func = [&]()
			{
				ofs << "void " << field_ptr->name() << "::to_json(flex_buffer& buffer)\n";
				ofs << "{\n";
				if (!field_ptr->fields().empty())
				{
					ofs << "\tjson::value jv{};\n";
					ofs << "\tauto& jv_obj = jv.emplace_object();\n";

					for (auto& [key, value] : field_ptr->fields())
					{
						if (generate_from_int(ofs, key, value, true))
							continue;

						if (generate_from_string(ofs, key, value, true))
							continue;

						if (generate_from_array(ofs, key, value, true))
							continue;

						generate_from_object(ofs, key, value, true);
					}
					ofs << "\tauto json_str = json::serialize(jv);\n";
					ofs << "\tbuffer.sputn(json_str.data(), json_str.size());\n";
				}

				ofs << "}\n";
			};

			auto from_func = [&]()
			{
				ofs << "void " << field_ptr->name() << "::from_json(flex_buffer& buffer)\n";
				ofs << "{\n";
				if (!field_ptr->fields().empty())
				{
					ofs << "\tjson::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));\n";
					ofs << "\tauto jv_obj = jv.try_as_object();\n";
					ofs << "\tif(jv_obj.has_error())\n";
					ofs << "\t{\n";
					ofs << "\t\treturn;\n";
					ofs << "\t}\n";

					for (auto& [key, value] : field_ptr->fields())
					{
						if (generate_to_int(ofs, key, value, true))
							continue;

						if (generate_to_string(ofs, key, value, true))
							continue;

						if (generate_to_array(ofs, key, value, true))
							continue;

						generate_to_object(ofs, key, value, true);
					}
				}

				ofs << "}\n";
			};

			to_func();
			from_func();

			return true;
		}

		bool cpp_generator::generate_seialize_method_kv(std::fstream& ofs, std::shared_ptr<field_base> field_ptr)
		{
			auto to_func = [&]()
			{
				ofs << "void " << field_ptr->name() << "::to_kv(flex_buffer& buffer)\n";
				ofs << "{\n";
				if (!field_ptr->fields().empty())
				{
					int loop = 0;
					for (auto& field : field_ptr->fields())
					{
						if (loop++ == 0)
						{
							ofs << "\tbuffer.sputc('?');\n";
						}
						else
						{
							ofs << "buffer.sputc('=');\n";
						}
						ofs << "\tkv_parse::to_datas(impl_ptr_->" << field.second << ", buffer);\n";
					}
				}

				ofs << "}\n";
			};

			auto from_func = [&]()
			{
				ofs << "void " << field_ptr->name() << "::from_kv(flex_buffer& buffer)\n";
				ofs << "{\n";
				if (!field_ptr->fields().empty())
				{
					for (auto& field : field_ptr->fields())
					{
						ofs << "\timpl_ptr_->" << field.second << " = kv_parse::from_datas<" << field.first
							<< ">(buffer, impl_ptr_->" << field.second << ");\n";
					}
				}

				ofs << "}\n";
			};

			to_func();

			from_func();

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

		bool cpp_generator::generate_to_int(std::fstream& ofs, const std::string& type, const std::string& value,
											bool has_impl)
		{
			auto ty = check_type(type);

			if (ty != json_type::integer)
				return false;

			if (type == "int32" || type == "uint32" || type == "float" || type == "uint64")
			{
				ofs << "\t";

				if (has_impl)
				{
					ofs << "impl_ptr_->";
				}

				ofs << value << " = static_cast<" << type << ">(jv_obj->at(\"" << value << "\").as_";

				if (type == "int32" || type == "uint32" || type == "uint64")
					ofs << "int64";
				else if (type == "float")
					ofs << "double";

				ofs << "());" << std::endl;
			}
			else if (type == "int64" || type == "double" || type == "bool")
			{
				ofs << "\t";

				if (has_impl)
				{
					ofs << "impl_ptr_->";
				}

				ofs << value << " = jv_obj->at(\"" << value << "\").as_" << type << "();" << std::endl;
			}

			return true;
		}

		bool cpp_generator::generate_to_string(std::fstream& ofs, const std::string& type, const std::string& value,
											   bool has_impl)
		{
			auto ty = check_type(type);

			if (ty != json_type::string)
				return false;

			ofs << "\t";

			if (has_impl)
			{
				ofs << "impl_ptr_->";
			}

			ofs << value << " = static_cast<" << type << ">(jv_obj->at(\"" << value << "\").as_string());" << std::endl;

			return true;
		}

		bool cpp_generator::generate_to_array(std::fstream& ofs, const std::string& type, const std::string& value,
											  bool has_impl)
		{
			auto ty = check_type(type);

			if (ty != json_type::array)
				return false;

			ofs << "\t";

			if (has_impl)
			{
				ofs << "impl_ptr_->";
			}

			ofs << value << " = aquarius::json_value_to_array(jv_obj->at(\"" << value << "\"));" << std::endl;

			return true;
		}

		bool cpp_generator::generate_to_object(std::fstream& ofs, const std::string& type, const std::string& value,
											   bool has_impl)
		{
			auto ty = check_type(type);

			if (ty != json_type::object)
				return false;

			ofs << "\t";

			if (has_impl)
			{
				ofs << "impl_ptr_->";
			}

			ofs << value << " = aquarius::json::value_to<" << type << ">(jv_obj->at(\"" << value << "\"));"
				<< std::endl;

			return true;
		}

		bool cpp_generator::generate_from_int(std::fstream& ofs, const std::string& type, const std::string& value,
											  bool has_impl)
		{
			auto ty = check_type(type);

			if (ty != json_type::integer)
				return false;

			ofs << "\tjv_obj.emplace(\"" << value << "\", ";

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

			ofs << "\tjv_obj.emplace(\"" << value << "\", ";

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

			ofs << "\tjv_obj.emplace(\"" << value << "\", aquarius::json_value_from_array(";

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

			ofs << "\tjv_obj.emplace(\"" << value << "\", aquarius::json_value_from_object<" << type << ">(";

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