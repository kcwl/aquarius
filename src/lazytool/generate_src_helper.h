#pragma once
#include <string>
#include <fstream>
#include "data_field.h"

namespace aquarius
{
	namespace lazytool
	{
		void genrate_src(std::ofstream& ofs, const std::string& class_name,
						 const std::vector<std::pair<std::string, std::string>>& fields, const std::string& method,
						 const std::string& direction, bool has_type = false);
	} // namespace lazytool
} // namespace aquarius