#pragma once
#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include <cstring>


#pragma warning(disable : 4996)

constexpr std::size_t ip_back_proto = 100;

constexpr std::size_t ip_report_proto = 102;