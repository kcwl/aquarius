#include "data_field.h"

namespace aquarius
{
	namespace lazytool
	{
		void data_field::set_name(const std::string& name)
		{
			name_ = name;
		}

		std::string data_field::name() const
		{
			return name_;
		}

		std::vector<std::pair<std::string, std::string>> data_field::fields() const
		{
			return fields_;
		}

		bool data_field::add_field(const std::string& type, const std::string& value)
		{
			auto iter = std::find_if(fields_.begin(), fields_.end(),
									 [&] (const auto& field) { return field.second == value; });

			if (iter != fields_.end())
			{
				std::cout << "Already have this field" << std::endl;

				return false;
			}

			fields_.emplace_back(type, value);

			return false;
		}
	}
}