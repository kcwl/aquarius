#pragma once
#include "cpp_generater.h"
#include "json_tag.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			class structure_generate : public generater, public json_tag
			{
			public:
				structure_generate() = default;
				virtual ~structure_generate() = default;

			public:
				virtual generate_error visit(std::shared_ptr<parser> parser, std::ofstream& ofs_h,
											 std::ofstream& ofs_cpp) override;

			private:
				void generate_main_define(std::ofstream& ofs, std::shared_ptr<parser> parser);
			};
		}
		
	} // namespace lazytool
} // namespace aquarius