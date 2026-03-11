#pragma once
#include "field.h"
#include <vector>

namespace aquarius
{
	namespace lazytool
	{
		class data_field : public field
		{
		public:
			data_field(struct_type s = struct_type::structure);

			~data_field() = default;

		public:
			bool add_field(const std::string& type, const std::string& value);

			std::vector<std::pair<std::string, std::string>> fields() const;

		private:
			std::vector<std::pair<std::string, std::string>> fields_;
		};
	} // namespace lazytool
} // namespace aquarius