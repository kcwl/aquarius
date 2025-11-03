#include "json_tag.h"
#include "data_field.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			void json_tag::generate_to_tag_define(std::ofstream& ofs, std::shared_ptr<data_field> parser)
			{
				ofs << parser->name() << " tag_invoke(const aquarius::json::value_to_tag<" << parser->name()
					<< ">&, const aquarius::json::value& jv);" << CRLF;
			}

			void json_tag::generate_from_tag_define(std::ofstream& ofs, std::shared_ptr<data_field> parser)
			{
				ofs << "void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const "
					<< parser->name() << "& local);" << CRLF;
			}
			void json_tag::generate_to_tag(std::ofstream& ofs, std::shared_ptr<data_field> parser)
			{
				ofs << parser->name() << " tag_invoke(const aquarius::json::value_to_tag<" << parser->name()
					<< ">&, const aquarius::json::value& jv)" << CRLF;
				ofs << "{" << CRLF;
				ofs << "\t" << parser->name() << " result{};" << CRLF;
				ofs << "\tauto obj = jv.try_as_object();" << CRLF;
				ofs << "\tif(obj->empty())" << CRLF;
				ofs << "\t\treturn {};" << TWO_CRLF;

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

				ofs << "\treturn result;" << CRLF;

				ofs << "}" << TWO_CRLF;
			}

			void json_tag::generate_from_tag(std::ofstream& ofs, std::shared_ptr<data_field> parser)
			{
				ofs << "void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const "
					<< parser->name() << "& local)" << CRLF;
				ofs << "{" << CRLF;
				ofs << "\tauto& jv_obj = jv.emplace_object();" << CRLF;

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
				ofs << "}" << TWO_CRLF;
			}

			bool json_tag::generate_to_int(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::integer)
					return false;

				if (type == "int32" || type == "uint32" || type == "float" || type == "uint64")
				{
					ofs << "\tresult." << value << " = static_cast<" << type << ">(obj->at(\"" << value
						<< "\").as_";

					if (type == "int32" || type == "uint32" || type == "uint64")
						ofs << "int64";
					else if (type == "float")
						ofs << "double";

					ofs << "());" << CRLF;
				}
				else if (type == "int64" || type == "double" || type == "bool")
				{
					ofs << "\tresult." << value << " = obj->at(\"" << value
						<< "\").as_" << type << "();" << CRLF;
				}

				return true;
			}

			bool json_tag::generate_to_string(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::string)
					return false;

				ofs << "\tresult." << value << " = static_cast<" << type << ">(obj->at(\"" << value
					<< "\").as_string());" << CRLF;

				return true;
			}

			bool json_tag::generate_to_array(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::array)
					return false;

				ofs << "\tresult." << value << " = aquarius::json_value_to_array(obj->at(\""
					<< value << "\"));" << CRLF;

				return true;
			}

			bool json_tag::generate_to_object(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::object)
					return false;

				ofs << "\tresult." << value << " = aquarius::json::value_to<" << type << ">(obj->at(\"" << value
					<< "\"));" << CRLF;

				return true;
			}

			bool json_tag::generate_from_int(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::integer)
					return false;

				ofs << "\tjv_obj.emplace(\"" << value << "\", local." << value << ");" << CRLF;

				return true;
			}

			bool json_tag::generate_from_string(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::string)
					return false;

				ofs << "\tjv_obj.emplace(\"" << value << "\", local." << value << ");" << CRLF;

				return true;
			}

			bool json_tag::generate_from_array(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::array)
					return false;

				ofs << "\tjv_obj.emplace(\"" << value << "\", aquarius::json_value_from_array(local." << value
					<< "));" << CRLF;

				return true;
			}

			bool json_tag::generate_from_object(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::object)
					return false;

				ofs << "\tjv_obj.emplace(\"" << value << "\", aquarius::json_value_from_object<" << type << ">(local."
					<< value << "));" << CRLF;

				return true;
			}

			json_tag::json_type json_tag::check_type(const std::string& type)
			{
				if (type == "int32" || type == "uint32" || type == "int64" || type=="uint64" || type=="bool" || type == "float" || type == "double")
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
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius