#pragma once
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/ip/http/http_server_session.hpp>
#include <aquarius/ip/http/http_redirect.hpp>
#include <aquarius/ip/http/http_handler.hpp>
#include <aquarius/ip/http/http_router.hpp>

#define AQUARIUS_HTTP_HANDLER(__request, __response, __method)                                                    \
	AQUARIUS_CONTEXT_BY_HTTP(aquarius::http_server_session, __request, __response, __method,                           \
							 aquarius::http_router<aquarius::http_server_session>)

#define AQUARIUS_HTTP_REDIRECT_FOREVER(src, dest)                                                                      \
	AQUARIUS_CONTEXT_BY_REDIRECT(aquarius::http_server_session, aquarius::http_router<aquarius::http_server_session>,  \
								 aquarius::virgo::http_method::redirect, src, dest)