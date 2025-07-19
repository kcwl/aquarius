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

#define AQUARIUS_TRANSFER_CONTEXT_BY(protocol, error, method)                                                          \
	class method                                                                                                       \
	{                                                                                                                  \
	public:                                                                                                            \
		error visit(std::vector<char> buffer)                                                                          \
		{                                                                                                              \
			buffer_ = std::move(buffer);                                                                               \
			return handle();                                                                                           \
		}                                                                                                              \
		auto buffer()                                                                                                  \
		{                                                                                                              \
			return buffer_;                                                                                            \
		}                                                                                                              \
	protected:                                                                                                         \
		error handle();                                                                                                \
	private:                                                                                                           \
		std::vector<char> buffer_;                                                                                     \
	};                                                                                                                 \
	inline error method::handle()

#define AQUARIUS_STREAM_CONTEXT(method, __rpc)                                                                         \
	AQUARIUS_STREAM_CONTEXT_BY(aquarius::ip::server_tcp_protocol, aquarius::error_code, method, __rpc)

#define AQUARIUS_TRANSFER_CONTEXT()                                                                                    \
	AQUARIUS_TRANSFER_CONTEXT_BY(aquarius::ip::server_tcp_protocol, aquarius::error_code, transfer_handle)
