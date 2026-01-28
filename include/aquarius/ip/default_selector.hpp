#pragma once
#include <aquarius/ip/http/http_method_define.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/router.hpp>
#include <aquarius/virgo/header_fields.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_version.hpp>

namespace aquarius
{
	namespace ip
	{
		struct tcp_selector
		{
			template <typename Session>
			auto operator()(std::string_view router_, std::shared_ptr<Session> session_ptr, virgo::header_fields& hf,
							flex_buffer& buffer)
			{
				router<Session>::get_mutable_instance().invoke(router_, session_ptr, hf, buffer);
			};
		};

		struct http_selector
		{
			http_selector(virgo::http_method m, virgo::http_version v, const std::string& url_path = "")
				: method(m)
				, version(v)
				, path(url_path)
			{}

			template <typename Session>
			auto operator()(std::string_view router_, std::shared_ptr<Session> session_ptr, virgo::header_fields& hf,
							flex_buffer& buffer)
			{
				std::string final_router(router_);

				auto content_type = hf.content_type();

				if ((content_type == "application/json" || content_type == "text/html" || content_type == "text/css") &&
					method == virgo::http_method::get)
				{
					final_router = __get_handler__;
				}
				else if (method == virgo::http_method::get)
				{
					if (!path.empty())
					{
						buffer.sputn(path.data(), path.size());
					}
				}
				else if (method == virgo::http_method::options)
				{
					final_router = __options_handler__;
				}

				router<Session>::get_mutable_instance().invoke(final_router, session_ptr, hf, buffer);
			};

			virgo::http_method method;

			virgo::http_version version;

			std::string path;
		};
	} // namespace ip
} // namespace aquarius