#pragma once
#include "header.hpp"
#include "message/message.hpp"

namespace aquarius
{
	template<class Body, std::size_t Number>
	using response = msg::message<false, msg::aquarius_tcp_response_header, Body, Number>;
}

