#pragma once
#include <aquarius/config.hpp>
#include <aquarius/context/context.hpp>
#include <aquarius/context/handler.hpp>
#include <aquarius/error_code.hpp>

#define AQUARIUS_STREAM_CONTEXT_BY(protocol, error, method, __rpc)                                                     \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::context::auto_handler_register<protocol, __rpc, method>                          \
		__auto_register_##method(__rpc::id);                                                                           \
	AQUARIUS_STREAM_HANDLER(typename protocol::session, method, error, __rpc)

#define AQUARIUS_TRANSFER_CONTEXT_BY(protocol, error, method, __rpc)                                                   \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::context::auto_transfer_handler_register<protocol, method>                        \
		__auto_register_##method(__rpc::id);                                                                           \
	AQUARIUS_STREAM_HANDLER(typename protocol::session, method, error, __rpc)

#define AQUARIUS_STREAM_CONTEXT(method, __rpc)                                                                         \
	AQUARIUS_STREAM_CONTEXT_BY(aquarius::ip::server_tcp_protocol, aquarius::error_code, method, __rpc)

#define AQUARIUS_TRANSFER_CONTEXT()                                                                                    \
	AQUARIUS_TRANSFER_CONTEXT_BY(aquarius::ip::server_tcp_protocol, aquarius::error_code,                              \
								 aquarius::context::transfer_handler, aquarius::context::rpc_transfer)
