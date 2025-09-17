#pragma once
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/http_response.hpp>

namespace aquarius
{
	namespace virgo
	{
		struct http_options_protocol
		{
			using request = http_request<"/", int, int>;

			using response = http_response<"/", int, int>;
		};
	} // namespace virgo
} // namespace aquarius