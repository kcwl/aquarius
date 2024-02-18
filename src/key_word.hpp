#pragma once
#include <array>
#include <string_view>
#include <map>

using namespace std::string_view_literals;

namespace aquarius
{
	inline static const std::map<std::string, std::string> key_type = {
		{"int8","int8_t"},
		{"int16","int16_t"},
		{"int32","int32_t"},
		{"int64","int64_t"},
		{"uint8","uint8_t"},
		{"uint16","uint16_t"},
		{"uint32","uint32_t"},
		{"uint64","uint64_t"},
		{"float","float"},
		{"double","double"},
		{"bool","bool"},
		{"bytes","std::vector<uint8_t>"},
		{"string","std::string"}
	};

	inline static const std::map<std::string, std::string> key_word = {
		{"message", "struct"},
		{"package","namespace"}
	};

} // namespace aquarius