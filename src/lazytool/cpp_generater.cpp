#include "cpp_generater.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			bool generater::generator_array_stream(std::ofstream& ofs, const std::string& type, const std::string& name)
			{
				if (!type.contains("bytes"))
					return false;

				ofs << "\"[\";\n";
				ofs << "\tfor (auto& s : other." << name << ")\n";
				ofs << "\t{\n";
				ofs << "\t\tos << s << \", \";\n";
				ofs << "\t}\n";

				ofs << "\tos.seekp(-2, std::ios::cur);\n";
				ofs << "\tos << \"]\";\n";

				return true;
			}

			void generater::generator_normal_stream(std::ofstream& ofs, const std::string& name)
			{
				ofs << "other." << name;
			}
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius