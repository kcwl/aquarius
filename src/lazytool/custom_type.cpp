#include "custom_type.h"
#include <map>
#include "file_parse.h"

namespace
{
	static std::map<std::string, std::shared_ptr<aquarius::lazytool::data_field>> custom_type;

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
		static std::map<std::string, std::shared_ptr<custom>> custom_type;
		void put_custom_type(const std::string& type, std::shared_ptr<data_field> data_ptr)
		{
			if (custom_type.find(type) != custom_type.end())
				return;

			auto ptr = std::make_shared<custom>(false, data_ptr);

			custom_type.emplace(type, ptr);
		}

		std::shared_ptr<custom> get_custom_type(const std::string& type)
		{
			auto iter = custom_type.find(type);
			if (iter == custom_type.end())
				return nullptr;
			return iter->second;
		}

		void custom_generate_json(const std::string& name)
		{
			auto custom = get_custom_type(name);

			if (!custom)
				return;

			custom->has_generate_json = true;
		}
	} // namespace lazytool
} // namespace aquarius