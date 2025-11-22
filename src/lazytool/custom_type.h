#pragma once
#include <string>
#include <memory>

namespace aquarius
{
	namespace lazytool
	{
		class data_field;

		struct custom
		{
			bool has_generate_json;

			std::shared_ptr<data_field> data_ptr;
		};
		
		void put_custom_type(const std::string& type, std::shared_ptr<data_field> data_ptr);

		std::shared_ptr<custom> get_custom_type(const std::string& type);

		void custom_generate_json(const std::string& name);
	} // namespace layztool
} // namespace aquarius
