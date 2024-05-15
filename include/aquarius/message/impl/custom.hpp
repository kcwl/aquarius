#pragma once
#include <cstdint>
#include <aquarius/core/elastic.hpp>

namespace aquarius
{
	class custom_tcp_request_header
	{
	public:
		custom_tcp_request_header() = default;
		virtual ~custom_tcp_request_header() = default;

		custom_tcp_request_header(custom_tcp_request_header&& other)
			: session_id(other.session_id)
		{
			other.session_id = 0;
		}

		custom_tcp_request_header(const custom_tcp_request_header&) = default;
		custom_tcp_request_header& operator=(const custom_tcp_request_header&) = default;

		ELASTIC_ACCESS(session_id);

	public:
		uint32_t session_id;
	};

	class custom_tcp_response_header : public custom_tcp_request_header
	{
	public:
		custom_tcp_response_header() = default;
		~custom_tcp_response_header() = default;

	public:
		custom_tcp_response_header(custom_tcp_response_header&& other)
			: custom_tcp_request_header(std::forward<custom_tcp_response_header>(other))
			, result(other.result)
		{
			other.result = 0;
		}

		custom_tcp_response_header(const custom_tcp_response_header&) = default;
		custom_tcp_response_header& operator=(const custom_tcp_response_header&) = default;


		ELASTIC_ACCESS_IF(custom_tcp_request_header, result);

	public:
		uint32_t result;
	};
} // namespace aquarius