#pragma once
#include <aquarius/context/handler.hpp>
#include <aquarius/context/transfer_context.hpp>
#include <aquarius/detail/context_base.hpp>
#include <aquarius/ip/tcp.hpp>

#define AQUARIUS_STREAM_CONTEXT_BY(session, error, method, __rpc)                                                      \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::context::auto_handler_register<session, __rpc, method> __auto_register_##method( \
		__rpc::id);                                                                                                    \
	AQUARIUS_STREAM_HANDLER(method, error, __rpc)

#define AQUARIUS_TRANSFER_CONTEXT_BY(session, error, method, __rpc)                                                    \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::context::auto_transfer_handler_register<session, method>                         \
		__auto_register_##method(__rpc::id);                                                                           \
	AQUARIUS_STREAM_HANDLER(method, error, __rpc)

#define AQUARIUS_STREAM_CONTEXT(method, __rpc)                                                                         \
	AQUARIUS_STREAM_CONTEXT_BY(aquarius::tcp::server_session, int, method, __rpc)

#define AQUARIUS_TRANSFER_CONTEXT()                                                                                    \
	AQUARIUS_TRANSFER_CONTEXT_BY(aquarius::tcp::server_session, int, transfer_handler, aquarius::rpc_transfer)
