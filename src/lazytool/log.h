#pragma once
#include <string>

namespace aquarius
{
	namespace virgo
	{
		std::string log(const std::string& func_name, const std::string& str, std::size_t column, std::size_t row);
	} // namespace virgo
} // namespace aquarius
