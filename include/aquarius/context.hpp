#pragma once
#include <aquarius/config.hpp>
#include <aquarius/context/context.hpp>
#include <aquarius/context/handler.hpp>

#define AQUARIUS_CONTEXT_BY(protocol, method, __rpc)                                                                   \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::context::auto_handler_register<protocol, __rpc, method>                          \
		__auto_register_##method(__rpc::id);                                                                           \
	AQUARIUS_POSITIVE_HANDLER(typename protocol::session, method, __rpc::request, __rpc::response)

#define AQUARIUS_CONTEXT(method, __rpc) AQUARIUS_CONTEXT_BY(aquarius::ip::server_tcp_protocol, method, __rpc)

#define AQUARIUS_RAW_CONTEXT()                                                                                         \
	AQUARIUS_HANDLER(aquarius::ip::server_tcp_protocol::session, transfer_handle, std::vector<char>)

#define AQUARIUS_RESPONSE(method, __rpc)                                                                               \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::context::auto_handler_register<aquarius::ip::client_tcp_protocol, __rpc, method> \
		__auto_register_##method(__rpc::id);                                                                           \
	AQUARIUS_HANDLER(aquarius::ip::client_tcp_protocol::session, method, __rpc::response)