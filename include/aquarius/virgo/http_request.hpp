#pragma once
#include <aquarius/ip/http/url_encode.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <format>

namespace aquarius
{
	namespace virgo
	{
		template <detail::string_literal Router, virgo::http_method Method, typename Header, typename Body>
		class http_request : public basic_http_protocol<true, Header, Body>
		{
		public:
			using base = basic_http_protocol<true, Header, Body>;

			using base::has_request;

			using base::json_type;

			using base::splitor;

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
			void consume(flex_buffer& buffer)
			{
				this->body().deserialize(buffer);
			}

			void commit(flex_buffer& buffer)
			{
				flex_buffer body_buffer{};

				std::string headline{};

				if constexpr (Method == virgo::http_method::get)
				{
					flex_buffer tempget;
					this->body().serialize(tempget);
					std::string temp_str((char*)tempget.data().data(), tempget.data().size());

					headline = std::format("{} {}{} {}\r\n", virgo::from_method_string(Method), router,
										   url_encode(temp_str), virgo::from_string_version(this->version()));
				}
				else
				{
					this->body().serialize(body_buffer);

					this->content_length(body_buffer.size());

					headline = std::format("{} {} {}\r\n", virgo::from_method_string(Method), router,
										   virgo::from_string_version(this->version()));
				}

				for (auto& s : this->fields())
				{
					headline += std::format("{}: {}\r\n", s.first, s.second);
				}

				headline += "\r\n";

				buffer.sputn(headline.c_str(), headline.size());

				if (body_buffer.size() != 0)
				{
					buffer.sputn((char*)body_buffer.data().data(), body_buffer.data().size());
				}
			}
		};

		template <detail::string_literal Router, virgo::http_method Method, typename Header, typename Body>
		std::ostream& operator<<(std::ostream& os, const http_request<Router, Method, Header, Body>& req)
		{
			req << os;

			return os;
		}
	} // namespace virgo

	template <detail::string_literal Router, virgo::http_method Method, typename Header, typename Body>
	struct is_message_type<virgo::http_request<Router, Method, Header, Body>> : std::true_type
	{};

	template <detail::string_literal Router, virgo::http_method Method, typename Header, typename Body>
	struct handler_tag_traits<virgo::http_request<Router, Method, Header, Body>>
	{
		constexpr static auto tag = proto_tag::http;
	};
} // namespace aquarius