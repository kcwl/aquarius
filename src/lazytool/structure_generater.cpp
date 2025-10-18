#include "structure_generater.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			structure_generate::generate_error structure_generate::visit(std::shared_ptr<parser> parser,
																		 std::ofstream& ofs_h, std::ofstream& ofs_cpp)
			{
				generate_main_define(ofs_h, parser);

				generate_operator_equal_src(ofs_cpp, parser);

				generate_operator_stream_src(ofs_cpp, parser);

				generate_to_tag(ofs_cpp, parser);

				generate_from_tag(ofs_cpp, parser);

				return generate_error::success;
			}

			void structure_generate::generate_main_define(std::ofstream& ofs, std::shared_ptr<parser> parser)
			{
				ofs << "struct " << parser->name_ << "\n";
				ofs << "{\n";
				for (auto& [type, name] : parser->values_)
				{
					ofs << "\t" << type << " " << name << ";\n";
				}
				ofs << "};\n";
			}
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius