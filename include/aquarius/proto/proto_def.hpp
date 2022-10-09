#pragma once
#include "header.hpp"
#include "ping.pb.h"
#include "../request.hpp"
#include "../response.hpp"

namespace aquarius
{
	using ping_request = request<tcp_request_header, ping_request_body, 100>;
	using ping_response = response<tcp_response_header, ping_response_body, 101>;
}

