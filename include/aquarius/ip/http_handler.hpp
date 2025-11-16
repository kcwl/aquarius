#pragma once
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/ip/http/http_server_session.hpp>
#include <aquarius/ip/http/http_redirect.hpp>
#include <aquarius/ip/http/http_handler.hpp>
#include <aquarius/ip/http/http_router.hpp>

#define AQUARIUS_HTTP_POST_HANDLER(__request, __response, __method)                                                    \
	AQUARIUS_CONTEXT_BY_HTTP(aquarius::http_server_session, __request, __response, __method,                           \
							 aquarius::http_router<aquarius::http_server_session>, aquarius::virgo::http_method::post)
#define AQUARIUS_HTTP_GET_HANDLER(__request, __response, __method)                                                     \
	AQUARIUS_CONTEXT_BY_HTTP(aquarius::http_server_session, __request, __response, __method,                           \
							 aquarius::http_router<aquarius::http_server_session>, aquarius::virgo::http_method::get)
#define AQUARIUS_HTTP_OPTIONS_HANDLER(__request, __response, __method)                                                 \
	AQUARIUS_CONTEXT_BY_HTTP(aquarius::http_server_session, __request, __response, __method,                           \
							 aquarius::http_router<aquarius::http_server_session>,                                     \
							 aquarius::virgo::http_method::options)
#define AQUARIUS_HTTP_PUT_HANDLER(__request, __response, __method)                                                     \
	AQUARIUS_CONTEXT_BY_HTTP(aquarius::http_server_session, __request, __response, __method,                           \
							 aquarius::http_router<aquarius::http_server_session>, aquarius::virgo::http_method::put)
#define AQUARIUS_HTTP_DELETE_HANDLER(__request, __response, __method)                                                  \
	AQUARIUS_CONTEXT_BY_HTTP(aquarius::http_server_session, __request, __response, __method,                           \
							 aquarius::http_router<aquarius::http_server_session>,                                     \
							 aquarius::virgo::http_method::delete_)
#define AQUARIUS_HTTP_HEAD_HANDLER(__request, __response, __method)                                                    \
	AQUARIUS_CONTEXT_BY_HTTP(aquarius::http_server_session, __request, __response, __method,                           \
							 aquarius::http_router<aquarius::http_server_session>, aquarius::virgo::http_method::head)

#define AQUARIUS_HTTP_REDIRECT_FOREVER(src, dest)                                                                      \
	AQUARIUS_CONTEXT_BY_REDIRECT(aquarius::http_server_session, aquarius::http_router<aquarius::http_server_session>,  \
								 aquarius::virgo::http_method::redirect, src, dest)