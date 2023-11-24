#pragma once
#include <array>
#include <string>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

namespace elastic
{
	enum class file_status
	{
		success,
		format_error,
	};

	enum class note_dir
	{
		left,
		right
	};

	enum class note_type
	{
		double_slash,
		slash
	};

	struct note
	{
		std::string content_;
		note_type type_;
	};

	struct reflactor_structure
	{
		note note_;

		std::string number_;

		std::string type_;

		std::string sub_type_;

		std::string name_;

		std::vector<reflactor_structure> structs_;
	};

	constexpr static std::array<std::string_view, 1> multi_key_words = { "message"sv };

	constexpr static std::array<std::string_view, 13> single_key_words = {
		"package"sv, "int32"sv,	  "int64"sv, "uint32"sv, "uint64"sv, "float"sv, "double"sv,
		"fixed32"sv, "fixed64"sv, "bool"sv,	 "bytes"sv,	 "string"sv, "map"sv
	};

	inline static const std::string tab = "\t";
	inline static const std::string crlf = "\r\n";
} // namespace elastic