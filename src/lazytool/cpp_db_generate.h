#pragma once
#include "model_parse.h"

namespace aquarius
{
	namespace lazytool
	{
		class cpp_db_generate
		{
		public:
			cpp_db_generate() = default;

		public:
			void generate_table(std::ofstream& ofs, std::shared_ptr<model_field> model_field_ptr);

		private:
			void generate_language(std::ofstream& ofs, const std::string& value);

			void generate_field(std::ofstream& ofs, std::shared_ptr<model_field> model_field_ptr);
		};
	} // namespace lazytool
} // namespace aquarius