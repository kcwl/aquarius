#pragma once
#include <string>
#include "tcp/message.hpp"

namespace aquarius
{
	template<class Body, std::size_t Number>
	using request = tcp::message<true, Body, Number>;
}