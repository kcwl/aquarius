#pragma once
#include "../request.hpp"
#include "../response.hpp"
#include "header.pb.h"
#include "ping.pb.h"

namespace aquarius
{
	using ping_request = request<packet_response_header, ping_request_body, 100>;
	using ping_response = response<packet_response_header, ping_response_body, 101>;
}

