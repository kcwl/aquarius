#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/http/http_helper.hpp>
#include <aquarius/resource/http_config.hpp>
#include <aquarius/virgo/http_header.hpp>
#include <expected>
#include <fstream>
#include <vector>

namespace aquarius
{
	inline error_code mpc_http_options(flex_buffer& buffer)
	{
		http_config& cfg = create_http();

		http_header header{};

		header.deserialize(buffer);

		http_header resp_header{};

		auto origin = header.find("Origin");

		if (origin.empty())
		{
			resp_header.set_field("Allow", cfg.control_allow_methods);
		}
		else
		{
			auto request_methods = header.find("Access-Control-Request-Method");

			auto request_headers = header.find("Access-Control-Request-Headers");

			if (!cfg.check_origin(origin) || !cfg.check_method(request_methods) || !cfg.check_headers(request_headers))
			{
				return http_status::forbidden;
			}

			resp_header.set_field("Access-Control-Allow-Origin", cfg.control_allow_origin);
			resp_header.set_field("Access-Control-Request-Methods", cfg.control_allow_methods);
			resp_header.set_field("Access-Control-Allow-Headers", cfg.control_allow_headers);
			resp_header.set_field("Access-Control-Max-Age", cfg.control_max_age);

			auto with_credential = header.find("withCredentials");
			if (!with_credential.empty())
			{
				resp_header.set_field("Access-Control-Allow-Credentials",
									  cfg.control_allow_credentials ? "true" : "false");
			}

			resp_header.serialize(buffer);
		}

		return http_status::no_content;
	}
} // namespace aquarius