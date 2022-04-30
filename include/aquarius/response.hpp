#pragma once
#include "message/message.hpp"
#include "header.hpp"

namespace aquarius
{
	template<class Body, std::size_t Number>
	using response = msg::message<false, Body, Number>;
}

