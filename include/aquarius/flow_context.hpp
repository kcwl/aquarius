#pragma once
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/handler.hpp>
#include <aquarius/flow/raw_buffer_flow.hpp>

#define AQUARIUS_CONTEXT_BY(__session, method, __rpc)                                                                  \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::detail::auto_handler_register<__session, __rpc, method>                          \
		__auto_register_##method(__rpc::id);                                                                           \
	AQUARIUS_POSITIVE_HANDLER(__session, method, __rpc::request, __rpc::response)

#define AQUARIUS_CONTEXT(method, __rpc) AQUARIUS_CONTEXT_BY(aquarius::tcp_server_session, method, __rpc)

#define AQUARIUS_RAW_CONTEXT()                                                                                         \
	AQUARIUS_CONTEXT_BY(aquarius::tcp_server_session, transfer_handle, aquarius::rpc_transfer_flow)

#define AQUARIUS_RESPONSE(method, __rpc)                                                                               \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::detail::auto_handler_register<aquarius::tcp_client_session, __rpc, method>       \
		__auto_register_##method(__rpc::id);                                                                           \
	AQUARIUS_HANDLER(aquarius::tcp_client_session, method, __rpc::response)