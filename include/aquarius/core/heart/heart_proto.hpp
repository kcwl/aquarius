#pragma once
#include <aquarius/proto/header.hpp>
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>

namespace aquarius
{
	namespace core
	{
		using ping_request = null_body_request<proto::tcp_request_header, 1000>;
		using ping_response = null_body_response<proto::tcp_response_header, 1001>;
	}
}