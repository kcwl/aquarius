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
		template <virgo::http_method Method, typename Body, virgo::http_version Version = virgo::http_version::http1_1>
		class http_response : public basic_http_protocol<false, Method, http_response_header, Body>
		{
		public:
			using base = basic_http_protocol<false, Method, http_response_header, Body>;

			using base::has_request;

		public:
			http_response() = default;

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

		protected:
			virtual std::string commit_header() override
			{
				std::string headerline = std::format("{} {} {}\r\n", virgo::from_string_version(Version),
													 static_cast<int>(this->header().result()),
													 virgo::from_status_string(this->header().result()));

				headerline += this->header_field();

				headerline += "\r\n";

				return headerline;
			}
		};

		template <virgo::http_method Method, typename Body, virgo::http_version Version>
		std::ostream& operator<<(std::ostream& os, const http_response<Method, Body, Version>& req)
		{
			req << os;

			return os;
		}
	} // namespace virgo

	template <virgo::http_method Method, typename Body, virgo::http_version Version>
	struct is_message_type<virgo::http_response<Method, Body, Version>> : std::true_type
	{};
} // namespace aquarius