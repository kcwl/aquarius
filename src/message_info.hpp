#pragma once
#include <array>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

constexpr std::array<std::string_view, 5> message_key = { "syntax"sv, "import"sv, "namespace"sv, "message"sv,"enum"sv };

constexpr std::array<std::string_view, 7> pod_type_array = { "int32"sv, "int64"sv, "uint32"sv,"uint64"sv,"float"sv, "bool"sv, "double"sv};

constexpr std::array<std::string_view, 5> complex_type_array = { "bytes"sv, "vector"sv, "oneof"sv, "map"sv };

constexpr std::string_view crlf = "\r\n"sv;

constexpr std::string_view protocol_syntax = "aquarius"sv;

constexpr char tab = '\t';

constexpr std::string_view suffix_str = "protocol"sv;

struct key_value
{
	std::string type;
	std::string type_name;
};

struct message_info : key_value
{
	std::vector<key_value> keys;
	std::string special_key;     //ÌØÊâ¹Ø¼ü×Ö
	std::string specila_number;
	std::vector<message_info> netests;
};