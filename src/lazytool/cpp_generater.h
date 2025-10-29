#pragma once
#include "generator.hpp"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			class generater : public generator
			{
			public:
				explicit generater() = default;
				~generater() = default;

			protected:
				bool generator_array_stream(std::ofstream& ofs, const std::string& type, const std::string& name);

				void generator_normal_stream(std::ofstream& ofs, const std::string& name);

				template<typename T>
					//requires std::is_base_of_v<parser, T>
				void generate_operator_equal_define(std::ofstream& ofs, std::shared_ptr<T> parser)
				{
					ofs << "\n";
					ofs << "bool operator==(const " << parser->name() << "& lhs, const " << parser->name() << "& rhs);\n";
				}

				template <typename T>
					//requires std::is_base_of_v<parser, T>
				void generate_operator_equal_src(std::ofstream& ofs, std::shared_ptr<T> parser)
				{
					ofs << "\n";
					ofs << "bool operator==(const " << parser->name() << "& lhs, const " << parser->name() << "& rhs)\n";
					ofs << "{\n";
					ofs << "\treturn ";
					for (auto& [_, name] : parser->fields())
					{
						ofs << "lhs." << name << " == rhs." << name << " && ";
					}

					ofs.seekp(-4, std::ios::cur);
					ofs << ";\n";
					ofs << "}\n\n";
				}

				template <typename T>
					//requires std::is_base_of_v<parser, T>
				void generate_operator_stream_define(std::ofstream& ofs, std::shared_ptr<T> parser)
				{
					ofs << "\n";

					ofs << "std::ostream& operator<<(std::ostream& os, const " << parser->name() << "& other);\n";
				}
				template <typename T>
					//requires std::is_base_of_v<parser, T>
				void generate_operator_stream_src(std::ofstream& ofs, std::shared_ptr<T> parser)
				{
					ofs << "\n";

					ofs << "std::ostream& operator<<(std::ostream& os, const " << parser->name() << "& other)\n";
					ofs << "{\n";
					ofs << "\tos ";

					for (auto& [type, name] : parser->fields())
					{
						ofs << "<< ";

						if (generator_array_stream(ofs, type, name))
							continue;

						generator_normal_stream(ofs, name);
					}

					//ofs.seekp(-4, std::ios::cur);

					ofs << ";\n";

					ofs << "\treturn os;\n";

					ofs << "}\n";
				}

			public:
				std::string file_name_;

				std::string output_path_;
			};
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius