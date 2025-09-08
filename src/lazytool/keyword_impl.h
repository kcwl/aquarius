#pragma once
#include <map>
#include <string>
#include <set>

namespace aquarius
{
	namespace virgo
	{
		std::string from_type_string(const std::string& target)
		{
			std::string result{};

			if (target == "int32")
			{
				result = "int32_t";
			}
			else if (target == "int64")
			{
				result = "int64_t";
			}
			else if (target == "uint32")
			{
				result = "uint32_t";
			}
			else if (target == "uint64")
			{
				result = "uint64_t";
			}
			else if (target == "float")
			{
				result = "float";
			}
			else if (target == "double")
			{
				result = "double";
			}
			else if (target == "string")
			{
				result = "std::string";
			}
			else if (target == "bool")
			{
				result = "bool";
			}
			else if (target == "bytes")
			{
				result = "std::vector<char>";
			}

			return result;
		}

		inline std::set<std::string> custom_type;

		std::string find_custom_type(const std::string& target)
		{
			auto iter = custom_type.find(target);
			if (iter == custom_type.end())
				return {};

			return *iter;
		}

		void put_custom_type(const std::string& target)
		{
			custom_type.insert(target);
		}

		bool check_type(const std::string& target)
		{
			return !from_type_string(target).empty() || !find_custom_type(target).empty();
		}

		constexpr static auto router_list = { "tcp-key", "http-str" };

		bool check_router_string(const std::string& target)
		{
			return std::find(router_list.begin(), router_list.end(), target) != router_list.end();
		}
	} // namespace virgo
} // namespace aquarius
