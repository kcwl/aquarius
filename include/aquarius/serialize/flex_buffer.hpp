#pragma once
#include <vector>
#include <expected>
#include <string>
#include <aquarius/serialize/error.hpp>
#include <boost/asio/streambuf.hpp>

namespace aquarius
{
	using flex_buffer = boost::asio::streambuf;
} // namespace aquarius