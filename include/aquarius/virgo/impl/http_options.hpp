#pragma once
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/http_response.hpp>
#include <aquarius/virgo/null_header.hpp>
#include <aquarius/virgo/null_body.hpp>

namespace aquarius
{
	namespace virgo
	{
		struct http_options_protocol
		{
			constexpr static std::string_view root = "/"sv;

			using request = http_request<root, null_header, null_body>;

			using response = http_response<root, null_header, null_body>;
		};
	} // namespace virgo
} // namespace aquarius