#pragma once
#include <string>
#include <vector>

namespace elastic
{
	namespace compiler
	{
		enum class keyword_type : int
		{
			none_t,
			include_t,
			namespace_t,
			enumclass_t,
			message_t,
		};

		struct header_file
		{
			std::vector<std::string> list_;
		};

		struct struct_member
		{
			std::string property_;
			std::string type_;
			std::string name_;
			int flag_;
		};

		struct struct_impl
		{
			std::string name_;
			std::string type_;
			keyword_type kt_;
			std::string default_type_;
			std::vector<struct_member> member_list_;
			std::vector<int> require_list_;
		};

		struct struct_file
		{
			std::vector<struct_impl> list_;
		};
	}
}
