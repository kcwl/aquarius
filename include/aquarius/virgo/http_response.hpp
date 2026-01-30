#pragma once
#include <aquarius/ip/concept.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_version.hpp>

namespace aquarius
{
	namespace virgo
	{

		template <virgo::http_method Method, typename Body>
		class http_response : public basic_http_protocol<false, http_response_header, Body>
		{
		public:
			using base = basic_http_protocol<false, http_response_header, Body>;

			using base::has_request;

		public:
			http_response() = default;
			http_response(virgo::header_fields f)
				: base(std::move(f))
			{}

			virtual ~http_response() = default;

			http_response(const http_response&) = delete;
			http_response& operator=(const http_response&) = delete;

			http_response(http_response&&) noexcept = default;
			http_response& operator=(http_response&&) noexcept = default;

		public:
			bool operator==(const http_response& other) const
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
				flex_buffer header_buffer{};
				this->header().serialize(header_buffer);

				this->serialize_custom_header(header_buffer);

				flex_buffer body_buffer{};

				this->body().serialize(body_buffer);

				if (body_buffer.size() != 0)
				{
					this->content_length(body_buffer.size());
				}

				//auto ver = co_await mpc_http_version();
				auto ver = "HTTP/1.1"sv;

				std::string headline =
					std::format("{} {} {}\r\n", ver,
								static_cast<int>(this->header().result()), virgo::from_status_string(this->header().result()));

				headline += this->header_field();

				headline += "\r\n";

				buffer.sputn(headline.c_str(), headline.size());

				if (body_buffer.size() != 0)
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

		template <virgo::http_method Method, typename Body>
		std::ostream& operator<<(std::ostream& os, const http_response<Method, Body>& req)
		{
			req << os;

			return os;
		}
	} // namespace virgo

	template <virgo::http_method Method, typename Body>
	struct is_message_type<virgo::http_response<Method, Body>> : std::true_type
	{};
} // namespace aquarius