#pragma once
#include <aquarius/context/auto_context.hpp>
#include <aquarius/context/handler.hpp>
#include <aquarius/ip/tcp.hpp>

#define AQUARIUS_TRANSFER_HANDLER(method, buffer)                                                                      \
	class method : public aquarius::basic_handler<buffer, int>                                                         \
	{                                                                                                                  \
	public:                                                                                                            \
		using request_type = buffer;                                                                                   \
		using base_type = aquarius::basic_handler<buffer, int>;                                                        \
                                                                                                                       \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##buffer))                                                    \
		{}                                                                                                             \
                                                                                                                       \
	protected:                                                                                                         \
		virtual auto handle() -> aquarius::awaitable<int> override;                                                    \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<int>

#define AQUARIUS_STREAM_CONTEXT_BY(session, error, method, __rpc)                                                      \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::context::auto_handler_register<session, __rpc, method> __auto_register_##method( \
		__rpc::id);                                                                                    \
	class method : public aquarius::stream_handler<__rpc::request, __rpc::response, error>                             \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::stream_handler<__rpc::request, __rpc::response, error>;                            \
                                                                                                                       \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__rpc))                                                     \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<error> override;                                                  \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<error>

#define AQUARIUS_STREAM_CONTEXT(method, __rpc) AQUARIUS_STREAM_CONTEXT_BY(aquarius::tcp::server_session, int, method, __rpc)

#define AQUARIUS_TRANSFER_CONTEXT_BY(session)                                                                          \
	AQUARIUS_TRANSFER_HANDLER(transfer_handler, std::vector<char>)                                                     \
	template <>                                                                                                        \
	struct basic_transfer_context<transfer_handler>                                                                    \
	{                                                                                                                  \
		static auto invoke(std::vector<char> buff, std::shared_ptr<session> session_ptr)                               \
		{                                                                                                              \
			static transfer_handler handler;                                                                           \
			auto resp = handler.visit(buff);                                                                           \
			session_ptr->send(resp);                                                                                   \
		}                                                                                                              \
	};

#define AQUARIUS_TRANSFER_CONTEXT() AQUARIUS_TRANSFER_CONTEXT_BY(aquarius::tcp::server_session)