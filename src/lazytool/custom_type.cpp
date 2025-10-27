#include "custom_type.h"

namespace
{
	static std::set<std::string> custom_type;

	constexpr static auto router_list = { "tcp-key", "http-str" };

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
		else if (target == "string")
		{
			result = "std::string";
		}
		else if (target == "bytes")
		{
			result = "std::vector<char>";
		}
		else
		{
			result = target;
		}

		return result;
	}
} // namespace

namespace aquarius
{
	namespace lazytool
	{
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

		bool check_router_string(const std::string& target)
		{
			return std::find(router_list.begin(), router_list.end(), target) != router_list.end();
		}
	} // namespace lazytool
} // namespace aquarius