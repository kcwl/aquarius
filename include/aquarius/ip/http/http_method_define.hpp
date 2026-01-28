#pragma once
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	constexpr static auto __get_handler__ = "__http_get_handler"sv;
	constexpr static auto __options_handler__ = "__http_options_handler"sv;
} // namespace aquarius