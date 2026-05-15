#pragma once
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/virgo/http_header.hpp>

namespace aquarius
{
	template <typename Body>
	class http_response : public basic_http_protocol<false, Body>
	{
	public:
		using base = basic_http_protocol<false, Body>;

		using base::has_request;

	public:
		http_response() = default;

		virtual ~http_response() = default;

		http_response(const http_response&) = default;

		http_response(http_response&&) noexcept = default;

		http_response& operator=(const http_response&) = default;

		http_response& operator=(http_response&&) noexcept = default;
	};
} // namespace aquarius