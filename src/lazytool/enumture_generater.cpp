#include "enumture_generater.h"
#include "enumture_parse.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			enumture_generate::generate_error enumture_generate::visit(std::shared_ptr<parser> parser,
																	   std::ofstream& ofs_h, std::ofstream&)
			{
				auto ptr = dynamic_pointer_cast<enumture_parse>(parser);

				if (!ptr)
					return generate_error::empty;

				if (ptr->values_.empty())
					return generate_error::empty;

				ofs_h << "enum class " << ptr->name_ << "\n";
				ofs_h << "{\n";
				for (auto& name : ptr->values_)
				{
					ofs_h << "\t" << name.first << ",\n";
				}
				ofs_h.seekp(-2, std::ios::cur);
				ofs_h << "\n";
				ofs_h << "};\n";

				return generate_error::success;
			}
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius