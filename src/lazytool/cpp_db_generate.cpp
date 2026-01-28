#include "cpp_db_generate.h"

namespace aquarius
{
	namespace lazytool
	{

		void cpp_db_generate::generate_table(std::ofstream& ofs, std::shared_ptr<model_field> model_field_ptr)
		{
			ofs << "struct " << model_field_ptr->name << std::endl;
			ofs << "{" << std::endl;
			ofs << "\t";
			generate_language(ofs, model_field_ptr->db_languase);

			generate_field(ofs, model_field_ptr);

			generate_member_func(ofs, model_field_ptr);

			generate_member_name_func(ofs, model_field_ptr);

			ofs << "};";
		}

		void cpp_db_generate::generate_language(std::ofstream& ofs, const std::string& value)
		{
			ofs << "constexpr static auto language = \"" << value << "\";\n";
		}

		void cpp_db_generate::generate_field(std::ofstream& ofs, std::shared_ptr<model_field> model_field_ptr)
		{
			for (auto& field : model_field_ptr->fields)
			{
				if (field.type.empty())
					continue;

				ofs << "\t" << field.type;

				auto pos = field.type.find_last_of(">");

				if (pos == std::string::npos)
				{
					ofs << "<";
				}
				else
				{
					ofs.seekp(-1, std::ios::cur);
				}

				if (!field.attrs.empty())
				{
					if(pos != std::string::npos)
					{
						ofs << ", ";
					}

					for (auto& attr : field.attrs)
					{
						ofs << attr << ", ";
					}

					ofs.seekp(-2, std::ios::cur);
				}

				ofs << "> ";

				ofs << field.name << ";" << std::endl;
			}
		}

		void cpp_db_generate::generate_member_func(std::ofstream& ofs, std::shared_ptr<model_field> model_field_ptr)
		{
			ofs << "\tconstexpr static auto member()\n";
			ofs << "\t{\n";
			ofs << "\t\treturn std::make_tuple(";
			for (auto& field : model_field_ptr->fields)
			{
				if (field.type.empty())
					continue;

				ofs << "\n\t\t\t&" << model_field_ptr->name << "::" << field.name << ",";
			}

			if (!model_field_ptr->fields.empty())
			{
				ofs.seekp(-1, std::ios_base::cur);
			}

			ofs << "\n\t\t);\n";
			ofs << "\t}\n";
		}

		void cpp_db_generate::generate_member_name_func(std::ofstream& ofs,
														std::shared_ptr<model_field> model_field_ptr)
		{
			ofs << "\tconstexpr static auto member_name()\n";
			ofs << "\t{\n";
			ofs << "\t\treturn std::make_tuple(\n";
			for (auto& field : model_field_ptr->fields)
			{
				if (field.type.empty())
					continue;

				ofs << "\n\t\t\t\"" << field.name << "\"sv,";
			}

			if (!model_field_ptr->fields.empty())
			{
				ofs.seekp(-1, std::ios_base::cur);
			}

			ofs << "\n\t\t);\n";
			ofs << "\t}\n";
		}
	} // namespace lazytool
} // namespace aquarius