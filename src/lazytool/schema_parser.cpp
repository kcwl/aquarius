#include "schema_parser.h"

namespace aquarius
{
	namespace lazytool
	{
		std::string schema_parser::log(const std::string& func_name, const std::string& str, std::size_t column, std::size_t row)
		{
			return std::format("[{}] {} at [row:{}, column:{}]", func_name, str, row, column);
		}

		void schema_parser::put_custom_type(const std::string& type)
		{
			custom_types_.insert(type);
		}

		bool schema_parser::check_custom_type(const std::string& type)
		{
			return custom_types_.find(type) != custom_types_.end();
		}
	}
}