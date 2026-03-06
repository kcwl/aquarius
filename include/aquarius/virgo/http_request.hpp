#pragma once
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/ip/concept.hpp>
#include <aquarius/ip/http/http_header.hpp>
#include <aquarius/ip/http/url_encode.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <format>

namespace aquarius
{
	struct http_selector;
}

namespace aquarius
{
	namespace virgo
	{
		template <detail::string_literal Router, virgo::http_method Method, typename Body,
				  virgo::http_version Version = virgo::http_version::http1_1>
		class http_request : public basic_http_protocol<true, Method, Body>
		{
		public:
			using base = basic_http_protocol<true, Method, Body>;

			using base::has_request;

			constexpr static auto this_router = detail::bind_param<Router>::value;

		public:
			http_request() = default;

			virtual ~http_request() = default;

			http_request(const http_request&) = default;
			http_request& operator=(const http_request&) = default;

			http_request(http_request&&) noexcept = default;
			http_request& operator=(http_request&&) noexcept = default;

		protected:
			virtual void commit_command_header(flex_buffer& buffer) override
			{
				std::string get_url(this_router);

				if constexpr (Method == virgo::http_method::get)
				{
					flex_buffer buf{};

					this->body().serialize(buf);

					get_url += std::string((char*)buf.data().data(), buf.size());
				}

				auto line = std::format("{} {} {}\r\n", virgo::from_method_string(Method), get_url,
										virgo::from_string_version(Version));

				buffer.sputn(line.c_str(), line.size());
			}
		};
	} // namespace virgo

	template <detail::string_literal Router, virgo::http_method Method, typename Body, virgo::http_version Version>
	struct is_message_type<virgo::http_request<Router, Method, Body, Version>> : std::true_type
	{};
} // namespace aquarius