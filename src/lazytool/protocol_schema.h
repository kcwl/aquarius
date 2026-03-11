#pragma once
#include "message_field.h"
#include "schema_parser.h"
#include <functional>
#include <map>

namespace aquarius
{
	namespace lazytool
	{
		class protocol_schema : public schema_parser
		{
			using func_t = std::function<void(std::fstream&, const std::string&)>;

		public:
			protocol_schema();

		public:
			virtual std::vector<std::shared_ptr<field>> run(const std::string& file_path,
															const std::string& protocol) override;

		private:
			bool parse_data_field(std::fstream& ifs, std::shared_ptr<data_field> field, bool anonymous = false,
								  bool has_key = true);

			bool parse_router_field(std::fstream& ifs, std::shared_ptr<message_field> field_ptr);

			bool parse_message_field(std::fstream& ifs, std::shared_ptr<message_field> field_ptr);

			bool parse_model(std::fstream& ifs, std::shared_ptr<data_field> field_ptr);

			bool check_type(const std::string& value);

			func_t invoke(const std::string& struct_name);

		private:
			std::vector<std::shared_ptr<field>> fields_;

			std::map<std::string, func_t> invokes_;
		};
	} // namespace lazytool
} // namespace aquarius