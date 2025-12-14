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
			ofs << "constexpr static auto language = \"" << value << "\"\n";
		}

		void cpp_db_generate::generate_field(std::ofstream& ofs, std::shared_ptr<model_field> model_field_ptr)
		{
			int field_number = 0;

			for (auto& field : model_field_ptr->fields)
			{
				ofs << "field<" << field_number << ", " << model_field_ptr->name << ", " << field.type << "> " << field.name << std::endl;
			}
		}

		void cpp_db_generate::generate_field_template(std::ofstream& ofs, std::shared_ptr<model_field> model_field_ptr)
		{
			int field_number = 0;

			for (auto& field : model_field_ptr->fields)
			{
				ofs << "template<>" << std::endl;
				ofs << "struct field<" << field_number <<", "<< model_field_ptr->name <<", " << field.type <<">" << std::endl;
				ofs << "{" << std::endl;
				ofs << "\tusing value_type = " << field.type << ";" << std::endl;
				ofs << "\tusing attr_type = std::tuple<";

				bool start = true;

				for (auto& attr : field.attrs)
				{
					if (!start)
						ofs << ", ";

					ofs << attr;
				}

				ofs << ">;" << std::endl;
				ofs << "\tstatic_assert(unique_index_adaptor<attr_type>(), \"unique index must be use on unique key\");" << std::endl;
				ofs << "\tstatic_assert(text_index_adaptor<attr_type>(), \"text index must be use on text key\");" << std::endl;
				ofs << "};";
			}
		}
	}
}