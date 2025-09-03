#pragma once
#include <virgo/detail/config.hpp>
#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <system_error>

namespace aquarius
{
	namespace virgo
	{
		enum class status : uint32_t
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

		inline std::string_view get_http_status_string(status s)
		{
			static const std::map<status, std::string_view> statuses = {
				{ status::unknown, "unknown" },
				{ status::continue_, "Continue" },
				{ status::switching_protocols, "Switching Protocols" },
				{ status::processing, "Processing" },
				{ status::early_hints, "Early Hints" },
				{ status::ok, "OK" },
				{ status::created, "Created" },
				{ status::accepted, "Accepted" },
				{ status::non_authoritative_information, "Non Authoritative Information" },
				{ status::no_content, "No Content" },
				{ status::reset_content, "Reset Content" },
				{ status::partial_content, "Partial Content" },
				{ status::multi_status, "Multi Status" },
				{ status::already_reported, "Already Reported" },
				{ status::im_used, "IM Used" },
				{ status::multiple_choices, "Multiple Choices" },
				{ status::moved_permanently, "Moved Permanently" },
				{ status::found, "Found" },
				{ status::see_other, "See Other" },
				{ status::not_modified, "Not Modified" },
				{ status::use_proxy, "Use Proxy" },
				{ status::temporary_redirect, "Temporary Redirect" },
				{ status::permanent_redirect, "Permanent Redirect" },
				{ status::bad_request, "Bad Request" },
				{ status::unauthorized, "Unauthorized" },
				{ status::payment_required, "Payment Required" },
				{ status::forbidden, "Forbidden" },
				{ status::not_found, "Not Found" },
				{ status::method_not_allowed, "Method Not Allowed" },
				{ status::not_acceptable, "Not Acceptable" },
				{ status::proxy_authentication_required, "Proxy Authentication Required" },
				{ status::request_timeout, "Request Timeout" },
				{ status::conflict, "Conflict" },
				{ status::gone, "Gone" },
				{ status::length_required, "Length Required" },
				{ status::precondition_failed, "Precondition Failed" },
				{ status::payload_too_large, "Payload Too Large" },
				{ status::uri_too_long, "Uri Too Long" },
				{ status::unsupported_media_type, "Unsupported Media Type" },
				{ status::range_not_satisfiable, "Range Not Satisfiable" },
				{ status::expectation_failed, "Expectation Failed" },
				{ status::i_am_a_teapot, "I'm a teapot" },
				{ status::misdirected_request, "Misdirected Request" },
				{ status::unprocessable_entity, "Unprocessable Entity" },
				{ status::locked, "Locked" },
				{ status::failed_dependency, "Failed Dependency" },
				{ status::too_early, "too_early" },
				{ status::upgrade_required, "upgrade_required" },
				{ status::precondition_required, "precondition_required" },
				{ status::too_many_requests, "too_many_requests" },
				{ status::request_header_fields_too_large, "request_header_fields_too_large" },
				{ status::unavailable_for_legal_reasons, "unavailable_for_legal_reasons" },
				{ status::internal_server_error, "internal_server_error" },
				{ status::not_implemented, "not_implemented" },
				{ status::bad_gateway, "bad_gateway" },
				{ status::service_unavailable, "service_unavailable" },
				{ status::gateway_timeout, "gateway_timeout" },
				{ status::http_version_not_supported, "http_version_not_supported" },
				{ status::variant_also_negotiates, "variant_also_negotiates" },
				{ status::insufficient_storage, "insufficient_storage" },
				{ status::loop_detected, "loop_detected" },
				{ status::not_extended, "not_extended" },
				{ status::network_authentication_required, "network_authentication_required" }
			};
			return statuses.at(s);
		}

		class http_statue_category : public std::error_category
		{
		public:
			constexpr http_statue_category() = default;

			[[nodiscard]] const char* name() const noexcept override
			{
				return "serviced errc";
			}

			[[nodiscard]] std::string message(int err_code) const override
			{
				return std::string(get_http_status_string(static_cast<status>(err_code)).data());
			}
		};

		template <typename ErrorCode>
		inline ErrorCode make_error_code(status result)
		{
			return ErrorCode(std::error_code(static_cast<int>(result), http_statue_category()));
		}

		enum class status_class : unsigned
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