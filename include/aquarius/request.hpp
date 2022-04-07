#pragma once
#include <string>
#include "message/message.hpp"

namespace aquarius
{
	template<class Body, std::size_t Number>
	using request = msg::message<true, Body, Number>;
}