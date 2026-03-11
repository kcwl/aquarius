#include "data_field.h"
#include <iostream>

namespace aquarius
{
	namespace lazytool
	{
		data_field::data_field(struct_type s)
			: field(s)
			, fields_()
		{}

		bool data_field::add_field(const std::string& type, const std::string& value)
		{
			auto iter =
				std::find_if(fields_.begin(), fields_.end(), [&](const auto& field) { return field.second == value; });

			if (iter != fields_.end())
			{
				std::cout << "Already have this field" << std::endl;

				return false;
			}

			fields_.emplace_back(type, value);

			return false;
		}

		std::vector<std::pair<std::string, std::string>> data_field::fields() const
		{
			return fields_;
		}
	} // namespace lazytool
} // namespace aquarius