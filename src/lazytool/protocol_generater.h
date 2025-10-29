#pragma once
#include "cpp_generater.h"
#include "json_tag.h"

namespace aquarius
{
	namespace lazytool
	{
		class protocol_struct;
		class service_struct;

		namespace cpp
		{
			class protocol_generate : public generater, public json_tag
			{
			public:
				protocol_generate() = default;
				virtual ~protocol_generate() = default;

			public:
				virtual generate_error visit(std::shared_ptr<parser> parser, std::ofstream& ofs_h,
											 std::ofstream& ofs_cpp) override;

				void defined(std::shared_ptr<parser> parser, std::ofstream& ofs);

			private:
				generate_error generate_service_define(std::ofstream& ofs, const std::string& service,
													   const std::string& domin, std::shared_ptr<service_struct> parser,
													   const std::string& protocol_router_key);

				generate_error generate_service_src(std::ofstream& ofs, const std::string& service,
													const std::string& domin, std::shared_ptr<service_struct> parser,
													const std::string& protocol, const std::string& router_key);
			};
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius