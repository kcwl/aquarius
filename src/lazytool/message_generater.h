#pragma once
#include "cpp_generater.h"
#include "json_tag.h"

namespace aquarius
{
	namespace lazytool
	{

		class message_struct;

		namespace cpp
		{
			class message_generate : public generater, public json_tag
			{
				using generater::CRLF;
				using generater::TWO_CRLF;
			public:
				message_generate() = default;
				virtual ~message_generate() = default;

			public:
				virtual generate_error visit(std::shared_ptr<parser> parser, std::ofstream& ofs_h,
											 std::ofstream& ofs_cpp) override;

				void defined(std::shared_ptr<parser> parser, std::ofstream& ofs);

				void set_request(bool v);

				bool is_request() const;

			private:
				generate_error generate_main_define(std::ofstream& ofs, std::shared_ptr<message_struct> parser);

				generate_error generate_main_src(std::ofstream& ofs, std::shared_ptr<message_struct> parser);
			};
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius