#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/http_status.hpp>

BOOST_AUTO_TEST_SUITE(virgo)

BOOST_AUTO_TEST_CASE(status)
{
	{
		aquarius::error_code ec = aquarius::virgo::http_status::ok;

		BOOST_TEST(ec.message() == "OK");

		BOOST_TEST(ec.category().name() == "serviced errc");
	}

	{
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::unknown) == "Unknown");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::continue_) == "Continue");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::switching_protocols) == "Switching Protocols");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::processing) == "Processing");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::early_hints) == "Early Hints");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::ok) == "OK");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::created) == "Created");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::accepted) == "Accepted");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::non_authoritative_information) == "Non Authoritative Information");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::no_content) == "No Content");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::reset_content) == "Reset Content");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::partial_content) == "Partial Content");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::multi_status) == "Multi Status");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::already_reported) == "Already Reported");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::im_used) == "IM Used");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::multiple_choices) == "Multiple Choices");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::moved_permanently) == "Moved Permanently");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::found) == "Found");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::see_other) == "See Other");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::not_modified) == "Not Modified");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::use_proxy) == "Use Proxy");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::temporary_redirect) == "Temporary Redirect");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::permanent_redirect) == "Permanent Redirect");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::bad_request) == "Bad Request");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::unauthorized) == "Unauthorized");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::payment_required) == "Payment Required");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::forbidden) == "Forbidden");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::not_found) == "Not Found");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::method_not_allowed) == "Method Not Allowed");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::not_acceptable) == "Not Acceptable");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::proxy_authentication_required) == "Proxy Authentication Required");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::request_timeout) == "Request Timeout");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::conflict) == "Conflict");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::gone) == "Gone");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::length_required) == "Length Required");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::precondition_failed) == "Precondition Failed");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::payload_too_large) == "Payload Too Large");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::uri_too_long) == "Uri Too Long");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::unsupported_media_type) == "Unsupported Media Type");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::range_not_satisfiable) == "Range Not Satisfiable");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::expectation_failed) == "Expectation Failed");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::i_am_a_teapot) == "I'm a teapot");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::misdirected_request) == "Misdirected Request");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::unprocessable_entity) == "Unprocessable Entity");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::locked) == "Locked");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::failed_dependency) == "Failed Dependency");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::too_early) == "Too Early");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::upgrade_required) == "Upgrade Required");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::precondition_required) == "Precondition Required");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::too_many_requests) == "Too Many Requests");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::request_header_fields_too_large) == "Request Header Fields Too Large");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::unavailable_for_legal_reasons) == "Unavailable For Legal Reasons");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::internal_server_error) == "Internal Server Error");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::not_implemented) == "Not Implemented");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::bad_gateway) == "Bad Gateway");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::service_unavailable) == "Service Unavailable");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::gateway_timeout) == "Gateway Timeout");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::http_version_not_supported) == "Http Version Not Supported");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::variant_also_negotiates) == "Variant Also Negotiates");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::insufficient_storage) == "Insufficient Storage");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::loop_detected) == "Loop Detected");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::not_extended) == "Not Extended");
		BOOST_TEST(aquarius::virgo::from_status_string(aquarius::virgo::http_status::network_authentication_required) == "Network Authentication Required");
	}
}

BOOST_AUTO_TEST_SUITE_END()