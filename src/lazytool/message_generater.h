#pragma once
#include "cpp_generater.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			class message_generate : public generater
			{
			public:
				message_generate() = default;
				virtual ~message_generate() = default;

			public:
				virtual generate_error visit(std::shared_ptr<parser> parser, std::ofstream& ofs_h,
											 std::ofstream& ofs_cpp) override;

				void defined(std::shared_ptr<parser> parser, std::ofstream& ofs);
			};
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius