#pragma once
#include <map>
#include <string>
#include <set>

namespace aquarius
{
	namespace layztool
	{
		std::string find_custom_type(const std::string& target);

		void put_custom_type(const std::string& target);

		bool check_type(const std::string& target);

		bool check_router_string(const std::string& target);
	} // namespace layztool
} // namespace aquarius
