#pragma once
#include <aquarius/tcp/handler.hpp>
#include <aquarius/tcp/session.hpp>

#define AQUARIUS_CONTEXT(mode, method, __request, __response) \
[[maybe_unused]] static aquarius::tcp::auto_context_register<aquarius::tcp::session, __request, method>                    \
__auto_context_register_##method(mode);


#define AQUARIUS_STREAM_CONTEXT(method, __request, __response) \
AQUARIUS_CONTEXT(context_kind::tcp_stream, method, __request, __response) \
AQUARIUS_STREAM_HANDLER(method, __request, __response)

#define AQUARIUS_BROADCASE_CONTEXT(method, __request, __response) \
AQUARIUS_CONTEXT(context_kind::broadcast, method, __request, __response) \
AQUARIUS_BROADCAST_HANDLER(method, __request, __response)


#define AQUARIUS_TRANSFER_CONTEXT(method) \
AQUARIUS_CONTEXT(context_kind::transfer, method, aquarius::flex_buffer, void)\
AQUARIUS_DEFAULT_HANDLER(method, aquarius::flex_buffer)


