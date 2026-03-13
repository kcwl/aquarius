#pragma once
#include <boost/mysql/date.hpp>
#include <boost/mysql/datetime.hpp>
#include <boost/mysql/time.hpp>
#include <cstdint>
#include <string>
#include <vector>

template <typename T>
class fixed : public T
{};

using int32 = uint32_t;
using uint32 = uint32_t;
using sint32 = int32_t;
using int64 = uint64_t;
using uint64 = uint64_t;
using sint64 = int64_t;
using fixed32 = fixed<uint32_t>;
using fixed64 = fixed<uint64_t>;

using string = std::string;
using bytes = std::vector<char>;

using date = boost::mysql::date;
using datetime = boost::mysql::datetime;
using time = boost::mysql::time;