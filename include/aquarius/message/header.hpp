#pragma once
#include "detail/header.hpp"
#include "detail/header_fields.hpp"
#include "detail/field.hpp"

namespace aquarius
{
	namespace msg
	{
		using aquarius_tcp_request_header = detail::header<true, detail::request_header>;

		using aquarius_tcp_response_header = detail::header<false, detail::response_header>;

		using common_header = detail::tcp_header;
	}
}