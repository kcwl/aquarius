#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <system_error>
#include <boost/system/error_category.hpp>
#include <aquarius/error_code.hpp>

namespace aquarius
{
	namespace virgo
	{
		enum class http_status : uint32_t
		{
			unknown = 0,

			continue_ = 100,
			switching_protocols = 101,
			processing = 102,
			early_hints = 103,

			ok = 200,
			created = 201,
			accepted = 202,
			non_authoritative_information = 203,
			no_content = 204,
			reset_content = 205,
			partial_content = 206,
			multi_status = 207,
			already_reported = 208,
			im_used = 226,

			multiple_choices = 300,
			moved_permanently = 301,
			found = 302,
			see_other = 303,
			not_modified = 304,
			use_proxy = 305,
			temporary_redirect = 307,
			permanent_redirect = 308,

			bad_request = 400,
			unauthorized = 401,
			payment_required = 402,
			forbidden = 403,
			not_found = 404,
			method_not_allowed = 405,
			not_acceptable = 406,
			proxy_authentication_required = 407,
			request_timeout = 408,
			conflict = 409,
			gone = 410,
			length_required = 411,
			precondition_failed = 412,
			payload_too_large = 413,
			uri_too_long = 414,
			unsupported_media_type = 415,
			range_not_satisfiable = 416,
			expectation_failed = 417,
			i_am_a_teapot = 418,
			misdirected_request = 421,
			unprocessable_entity = 422,
			locked = 423,
			failed_dependency = 424,
			too_early = 425,
			upgrade_required = 426,
			precondition_required = 428,
			too_many_requests = 429,
			request_header_fields_too_large = 431,
			unavailable_for_legal_reasons = 451,

			internal_server_error = 500,
			not_implemented = 501,
			bad_gateway = 502,
			service_unavailable = 503,
			gateway_timeout = 504,
			http_version_not_supported = 505,
			variant_also_negotiates = 506,
			insufficient_storage = 507,
			loop_detected = 508,
			not_extended = 510,
			network_authentication_required = 511
		};

		inline std::string_view from_status_string(http_status s)
		{
			static std::map<http_status, std::string_view> statuses = {
				{ http_status::unknown, "Unknown" },
				{ http_status::continue_, "Continue" },
				{ http_status::switching_protocols, "Switching Protocols" },
				{ http_status::processing, "Processing" },
				{ http_status::early_hints, "Early Hints" },
				{ http_status::ok, "OK" },
				{ http_status::created, "Created" },
				{ http_status::accepted, "Accepted" },
				{ http_status::non_authoritative_information, "Non Authoritative Information" },
				{ http_status::no_content, "No Content" },
				{ http_status::reset_content, "Reset Content" },
				{ http_status::partial_content, "Partial Content" },
				{ http_status::multi_status, "Multi Status" },
				{ http_status::already_reported, "Already Reported" },
				{ http_status::im_used, "IM Used" },
				{ http_status::multiple_choices, "Multiple Choices" },
				{ http_status::moved_permanently, "Moved Permanently" },
				{ http_status::found, "Found" },
				{ http_status::see_other, "See Other" },
				{ http_status::not_modified, "Not Modified" },
				{ http_status::use_proxy, "Use Proxy" },
				{ http_status::temporary_redirect, "Temporary Redirect" },
				{ http_status::permanent_redirect, "Permanent Redirect" },
				{ http_status::bad_request, "Bad Request" },
				{ http_status::unauthorized, "Unauthorized" },
				{ http_status::payment_required, "Payment Required" },
				{ http_status::forbidden, "Forbidden" },
				{ http_status::not_found, "Not Found" },
				{ http_status::method_not_allowed, "Method Not Allowed" },
				{ http_status::not_acceptable, "Not Acceptable" },
				{ http_status::proxy_authentication_required, "Proxy Authentication Required" },
				{ http_status::request_timeout, "Request Timeout" },
				{ http_status::conflict, "Conflict" },
				{ http_status::gone, "Gone" },
				{ http_status::length_required, "Length Required" },
				{ http_status::precondition_failed, "Precondition Failed" },
				{ http_status::payload_too_large, "Payload Too Large" },
				{ http_status::uri_too_long, "Uri Too Long" },
				{ http_status::unsupported_media_type, "Unsupported Media Type" },
				{ http_status::range_not_satisfiable, "Range Not Satisfiable" },
				{ http_status::expectation_failed, "Expectation Failed" },
				{ http_status::i_am_a_teapot, "I'm a teapot" },
				{ http_status::misdirected_request, "Misdirected Request" },
				{ http_status::unprocessable_entity, "Unprocessable Entity" },
				{ http_status::locked, "Locked" },
				{ http_status::failed_dependency, "Failed Dependency" },
				{ http_status::too_early, "Too Early" },
				{ http_status::upgrade_required, "Upgrade Required" },
				{ http_status::precondition_required, "Precondition Required" },
				{ http_status::too_many_requests, "Too Many Requests" },
				{ http_status::request_header_fields_too_large, "Request Header Fields Too Large" },
				{ http_status::unavailable_for_legal_reasons, "Unavailable For Legal Reasons" },
				{ http_status::internal_server_error, "Internal Server Error" },
				{ http_status::not_implemented, "Not Implemented" },
				{ http_status::bad_gateway, "Bad Gateway" },
				{ http_status::service_unavailable, "Service Unavailable" },
				{ http_status::gateway_timeout, "Gateway Timeout" },
				{ http_status::http_version_not_supported, "Http Version Not Supported" },
				{ http_status::variant_also_negotiates, "Variant Also Negotiates" },
				{ http_status::insufficient_storage, "Insufficient Storage" },
				{ http_status::loop_detected, "Loop Detected" },
				{ http_status::not_extended, "Not Extended" },
				{ http_status::network_authentication_required, "Network Authentication Required" }
			};
			return statuses.at(s);
		}

		inline std::string_view from_status_string(int s)
		{
			return from_status_string(static_cast<http_status>(s));
		}

		class http_status_category : public boost::system::error_category
		{
		public:
			constexpr http_status_category() = default;

			[[nodiscard]] const char* name() const noexcept override
			{
				return "serviced errc";
			}

			[[nodiscard]] std::string message(int err_code) const override
			{
				return std::string(from_status_string(static_cast<http_status>(err_code)).data());
			}
		};

		static http_status_category& get_http_status_category()
		{
			static http_status_category category{};

			return category;
		}

		inline error_code make_error_code(http_status result)
		{
			return error_code(static_cast<int>(result), get_http_status_category());
		}

		enum class http_status_class : unsigned
		{
			unknown = 0,

			informational = 1,

			successful = 2,

			redirection = 3,

			client_error = 4,

			server_error = 5,
		};
	} // namespace virgo
} // namespace aquarius

namespace std
{
	template <>
	struct is_error_code_enum<aquarius::virgo::http_status> : std::true_type
	{};
} // namespace std