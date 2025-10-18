#pragma once
#include <string>

namespace aquarius
{
	namespace lazytool
	{
		std::string log(const std::string& func_name, const std::string& str, std::size_t column, std::size_t row);
	} // namespace lazytool
} // namespace aquarius
