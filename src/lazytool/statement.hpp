#pragma once
#include <optional>
#include <string>
#include <vector>

namespace virgo
{
	struct statement
	{
		std::string type;
		std::string name;
		std::optional<std::string> sub_type;
		std::optional<std::string> number;
		std::vector<statement> states;
	};
} // namespace virgo