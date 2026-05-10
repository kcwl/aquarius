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
	template<typename Session>
	inline auto mpc_http_options(std::shared_ptr<Session> session_ptr, flex_buffer& buffer) -> asio::awaitable<error_code>
	{
		http_header header{};

		header.deserialize(buffer);

		flex_buffer resp_buffer{};

		http_header resp_header{};

		auto origin = header.find("Origin");

		if (origin.empty())
		{
			resp_header.set_field("Allow", "GET,POST");
		}
		else
		{
			auto request_methods = header.find("Access-Control-Request-Methods");

			auto request_headers = header.find("Access-Control-Request-Headers");

			http_config& cfg = create_http();

			if (!cfg.check_origin(origin))
			{
				co_return http_status::forbidden;
			}

			if (!cfg.check_method(request_methods))
			{
				co_return http_status::forbidden;
			}

			if (!cfg.check_headers(request_headers))
			{
				co_return http_status::forbidden;
			}

			resp_header.set_field("Access-Control-Allow-Origin", cfg.control_allow_origin);
			resp_header.set_field("Access-Control-Request-Methods", cfg.control_allow_methods);
			resp_header.set_field("Access-Control-Allow-Headers", cfg.control_allow_headers);
			resp_header.set_field("Access-Control-Max-Age", cfg.control_max_age);
			resp_header.set_field("Access-Control-Allow-Credentials", cfg.control_allow_credentials ? "true" : "false");

			auto headline = make_response_command_header(http_status::no_content);

			resp_buffer.sputn(headline.c_str(), headline.size());

			resp_header.serialize(resp_buffer);
		}

		co_return co_await session_ptr->async_send(resp_buffer);
	}
} // namespace aquarius