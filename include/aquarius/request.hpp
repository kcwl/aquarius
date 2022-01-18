#pragma once
#include <string>
#include "tcp/message.hpp"

namespace aquarius
{
	using null_request = tcp::message<true, std::string, -1>;

	template<class Body, std::size_t Number>
	using request = tcp::message<true, Body, Number>;
}