#pragma once
#include "tcp/message.hpp"
#include "header.hpp"

namespace aquarius
{
	template<class Body, std::size_t Number>
	using response = tcp::message<false, Body, Number>;
}

