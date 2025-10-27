#pragma once
#include "cpp_generater.h"
#include "json_tag.h"

namespace aquarius
{
	namespace lazytool
	{
		class domin_parse;

		namespace cpp
		{
			class domin_generate : public generater, public json_tag
			{
			public:
				domin_generate() = default;
				virtual ~domin_generate() = default;

			public:
				virtual generate_error visit(std::shared_ptr<parser> parser, std::ofstream& ofs_h,
											 std::ofstream& ofs_cpp) override;

			private:
				void generate_main_define(std::ofstream& ofs, std::shared_ptr<domin_parse> parser);

				void generate_main_src(std::ofstream& ofs, std::shared_ptr<domin_parse> parser);
			};
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius