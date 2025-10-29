#pragma once
#include <string>
#include <vector>
#include <iostream>

namespace aquarius
{
	namespace lazytool
	{
		class data_field
		{
		public:
			data_field() = default;
			~data_field() = default;

		public:
			void set_name(const std::string& name);

			std::string name() const;

			std::vector<std::pair<std::string, std::string>> fields() const;

			bool add_field(const std::string& type, const std::string& value);

		private:
			std::string name_;

			std::vector<std::pair<std::string, std::string>> fields_;
		};
	} // namespace lazytool
} // namespace aquarius