#pragma once
#include <aquarius/elastic/access.hpp>
#include <aquarius/elastic/archive.hpp>
#include <aquarius/elastic/flex_buffer.hpp>
#include <aquarius/elastic/message_lite.hpp>
#include <aquarius/elastic/reflect.hpp>

using fixed32_t = aquarius::fixed<uint32_t>;
using fixed64_t = aquarius::fixed<uint64_t>;
using bytes = std::string;