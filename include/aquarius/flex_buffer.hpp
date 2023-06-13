#pragma once
#include <aquarius/detail/streambuf.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/access.hpp>

namespace aquarius
{
	using flex_buffer_t = aquarius::detail::basic_streambuf<char, std::char_traits<char>>;
}