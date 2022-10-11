#pragma once
#include "header.hpp"
#include "../request.hpp"
#include "../response.hpp"

namespace aquarius
{
	using ping_request = null_body_request<tcp_request_header, 1000>;
	using ping_response = null_body_response<tcp_response_header, 1001>;
}

