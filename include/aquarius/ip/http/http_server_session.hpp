#pragma once
#include <aquarius/ip/http/http.hpp>
#include <aquarius/basic_session.hpp>

namespace aquarius
{
	using http_server_session = basic_session<true, http>;

	using ssl_http_server_session = basic_session<true, http, detail::ssl_context_factory<true>>;
}