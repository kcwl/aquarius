#pragma once
#include "cpp_generater.h"
#include "json_tag.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			class normal_message_generate : public generater, public json_tag
			{
			public:
				normal_message_generate() = default;
				virtual ~normal_message_generate() = default;

			public:
				virtual generate_error visit(std::shared_ptr<parser> parser, std::ofstream& ofs_h,
											 std::ofstream& ofs_cpp) override;

				void defined(std::shared_ptr<parser> parser, std::ofstream& ofs);

			private:
				void generate_main_define(std::ofstream& ofs, std::shared_ptr<parser> parser);
			};
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius