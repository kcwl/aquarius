#include "log.h"
#include <format>

namespace virgo
{
	std::string log(const std::string& func_name, const std::string& str, std::size_t column, std::size_t row)
	{
		return std::format("[{}] {} at [column:{},row:{}]", func_name, str, column, row);
	}
} // namespace virgo