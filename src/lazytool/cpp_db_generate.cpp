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

				ofs << "\tfield<" << field.type;

				for (auto& attr : field.attrs)
				{
					ofs << ", " << attr;
				}
				
				ofs << "> " << field.name << ";" << std::endl;
			}
		}
	}
}