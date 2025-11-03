#include "structure_generater.h"
#include "structure_parse.h"

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

                auto ptr = std::dynamic_pointer_cast<structure>(parser);

                generate_operator_equal_define(ofs_h, ptr);

                generate_operator_stream_define(ofs_h, ptr);

                generate_operator_equal_src(ofs_cpp, ptr);

                generate_operator_stream_src(ofs_cpp, ptr);

                generate_to_tag_define(ofs_h, ptr);

                generate_from_tag_define(ofs_h, ptr);

                generate_to_tag(ofs_cpp, ptr);

                generate_from_tag(ofs_cpp, ptr);

                return generate_error::success;
            }

            structure_generate::generate_error structure_generate::generate_main_define(std::ofstream& ofs, std::shared_ptr<parser> parser)
            {
                auto ptr = dynamic_pointer_cast<structure>(parser);

                if (!ptr)
                    return generate_error::empty;

                if (ptr->fields().empty())
                    return generate_error::empty;

                ofs << "struct " << ptr->name() << "\n";
                ofs << "{\n";
                for (auto& [type, name] : ptr->fields())
                {
                    ofs << "\t" << type << " " << name << ";\n";
                }
                ofs << "};\n";

                return generate_error::success;
            }
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius