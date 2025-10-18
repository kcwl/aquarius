#include "json_tag.h"
#include "parser.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			void json_tag::generate_to_tag(std::ofstream& ofs, std::shared_ptr<parser> parser)
			{
				ofs << parser->name_ << " tag_invoke(const aquarius::json::value_to_tag<" << parser->name_
					<< ">&, const aquarius::json::value& jv)\n";
				ofs << "{\n";
				ofs << "\t" << parser->name_ << " result{};\n";
				ofs << "\tauto obj = jv.try_as_object();\n";
				ofs << "\tif(obj->empty())\n";
				ofs << "\t\treturn {};\n\n";

				for (auto& [key, value] : parser->values_)
				{
					if (generate_to_int(ofs, key, value))
						continue;

					if (generate_to_string(ofs, key, value))
						continue;

					if (generate_to_array(ofs, key, value))
						continue;

					generate_to_object(ofs, key, value);
				}

				ofs << "\treturn result;\n";

				ofs << "}\n\n";
			}

			void json_tag::generate_from_tag(std::ofstream& ofs, std::shared_ptr<parser> parser)
			{
				ofs << "void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const "
					<< parser->name_ << "& local)\n";
				ofs << "{\n";
				ofs << "\tauto& jv_obj = jv.emplace_object();\n";

				for (auto& [key, value] : parser->values_)
				{
					if (generate_from_int(ofs, key, value))
						continue;

					if (generate_from_string(ofs, key, value))
						continue;

					if (generate_from_array(ofs, key, value))
						continue;

					generate_from_object(ofs, key, value);
				}
				ofs << "}\n\n";
			}

			bool json_tag::generate_to_int(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::integer)
					return false;

				ofs << "\tresult." << value << " = static_cast<" << type << ">(obj->at(\"" << value
					<< "\").as_uint64());\n";

				return true;
			}

			bool json_tag::generate_to_string(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::string)
					return false;

				ofs << "\tresult." << value << " = static_cast<" << type << ">(obj->at(\"" << value
					<< "\").as_string());\n";

				return true;
			}

			bool json_tag::generate_to_array(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::array)
					return false;

				ofs << "\tresult." << value << " = aquarius::serialize::json_value_to_array(obj->at(\""
					<< value << "\"));\n";

				return true;
			}

			bool json_tag::generate_to_object(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::object)
					return false;

				ofs << "\tresult." << value << " = aquarius::json::value_to<" << type << ">(obj->at(\"" << value
					<< "\"));\n";

				return true;
			}

			bool json_tag::generate_from_int(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::integer)
					return false;

				ofs << "\tjv_obj.emplace(\"" << value << "\", local." << value << "); \n";

				return true;
			}

			bool json_tag::generate_from_string(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::string)
					return false;

				ofs << "\tjv_obj.emplace(\"" << value << "\", local." << value << "); \n";

				return true;
			}

			bool json_tag::generate_from_array(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::array)
					return false;

				ofs << "\tjv_obj.emplace(\"" << value << "\", aquarius::serialize::json_value_from_array(local." << value
					<< ")); \n";

				return true;
			}

			bool json_tag::generate_from_object(std::ofstream& ofs, const std::string& type, const std::string& value)
			{
				auto ty = check_type(type);

				if (ty != json_type::object)
					return false;

				ofs << "\tjv_obj.emplace(\"" << value << "\", aquarius::serialize::json_value_from_object<" << type << ">(local."
					<< value << ")); \n";

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