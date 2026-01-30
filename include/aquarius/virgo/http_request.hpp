#pragma once
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/ip/http/http_header.hpp>
#include <aquarius/ip/http/url_encode.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <format>

namespace aquarius
{
	namespace virgo
	{
		template <detail::string_literal Router, virgo::http_method Method, typename Body>
		class http_request : public basic_http_protocol<true, http_request_header, Body>
		{
			constexpr static auto crlf = "\r\n"sv;

		public:
			using base = basic_http_protocol<true, http_request_header, Body>;

			using base::has_request;

			constexpr static auto router = detail::bind_param<Router>::value;

		public:
			http_request() = default;

			virtual ~http_request() = default;

			http_request(const http_request&) = default;
			http_request& operator=(const http_request&) = default;

			http_request(http_request&&) noexcept = default;
			http_request& operator=(http_request&&) noexcept = default;

		public:
			bool operator==(const http_request& other) const
			{
				return base::operator==(other);
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				return base::operator<<(os);
			}

		public:
			void commit(flex_buffer& buffer)
			{
				this->seq_number(detail::uuid_generator()());

				flex_buffer header_buffer{};
				this->header().serialize(header_buffer);

				this->serialize_custom_header(header_buffer);

				flex_buffer body_buffer{};

				std::string headline{};

				this->body().serialize(body_buffer);

				if constexpr (Method == virgo::http_method::get)
				{
					auto get_url =
						boost::urls::url_view(std::string_view((char*)body_buffer.data().data(), body_buffer.size()));

					headline = std::format("{} {}{} {}\r\n", virgo::from_method_string(Method), router,
										   std::string(get_url.encoded_params().buffer().data(), get_url.encoded_params().buffer().size()), /*virgo::from_string_version(this->version())*/ "HTTP/1.1");
				}
				else
				{
					this->content_length(body_buffer.size());

					this->set_header("Content-Type", "application/json");

					headline = std::format("{} {} {}\r\n", virgo::from_method_string(Method), router,
										   /*virgo::from_string_version(this->version())*/"HTTP/1.1");
				}

				headline += this->header_field();

				headline += crlf;

				buffer.sputn(headline.data(), headline.size());

				if constexpr (Method != virgo::http_method::get)
				{
					buffer.sputn((char*)body_buffer.data().data(), body_buffer.data().size());
				}
			}

			void consume(flex_buffer& buffer)
			{
				auto buf = this->deserialize_custom_header();

				this->header().deserialize(buf);

				this->body().deserialize(buffer);
			}
		};

		template <detail::string_literal Router, virgo::http_method Method, typename Body>
		std::ostream& operator<<(std::ostream& os, const http_request<Router, Method, Body>& req)
		{
			req << os;

			return os;
		}
	} // namespace virgo

	template <detail::string_literal Router, virgo::http_method Method, typename Body>
	struct is_message_type<virgo::http_request<Router, Method, Body>> : std::true_type
	{};

	template <detail::string_literal Router, virgo::http_method Method, typename Body>
	struct handler_tag_traits<virgo::http_request<Router, Method, Body>>
	{
		constexpr static auto tag = proto::http;
	};
} // namespace aquarius