#pragma once
#include <string>
#include <vector>
#include <fstream>

namespace aquarius
{
	namespace lazytool
	{
		struct model_data_field
		{
			std::string type;

			std::string name;

			std::vector<std::string> attrs;
		};

		struct model_field
		{
			std::string db_languase;

			std::string name;

			std::vector<model_data_field> fields;
		};

		class model_parse
		{
		public:
			model_parse() = default;

		public:
			bool read_file(const std::string& file_path);

		private:
			bool parse_model(std::ifstream& ifs, std::shared_ptr<model_field> model_field_ptr);

		public:
			std::vector<model_field> models_;

			std::size_t column_;

			std::size_t row_;
		};
	} // namespace lazytool
} // namespace aquarius