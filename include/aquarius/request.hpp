#pragma once
#include "header.hpp"
#include "message/message.hpp"

namespace aquarius
{
	template<class Body, std::size_t Number>
	using request = msg::message<true, msg::aquarius_tcp_request_header, Body, Number>;
}