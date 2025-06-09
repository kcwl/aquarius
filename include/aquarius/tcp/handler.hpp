#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/tcp/auto_register.hpp>
#include <aquarius/tcp/session.hpp>
#include <aquarius/tcp/stream_handler.hpp>

#define AQUARIUS_GLOBAL_ID(request) #request

#define AQUARIUS_DEFAULT_HANDLER(method, __request)                                                                    \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::tcp::auto_handler_register<aquarius::tcp::server_session, __request, method>     \
		__auto_register_##method(__request::proto);                                                                    \
	class method : public aquarius::basic_handler<__request, int>                                                      \
	{                                                                                                                  \
	public:                                                                                                            \
		using request_type = __request;                                                                                \
		using base_type = aquarius::basic_handler<__request, int>;                                                     \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__handler_##__request))                                                     \
		{}                                                                                                             \
	protected:                                                                                                         \
		virtual auto handle() -> aquarius::awaitable<int> override;                                                    \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<int>

#define AQUARIUS_STREAM_HANDLER(method, __rpc)                                                                         \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::tcp::auto_handler_register<aquarius::tcp::server_session, __rpc::tcp_request,    \
																 method>                                               \
		__auto_register_##method(__rpc::tcp_request::proto);                                                           \
	class method : public aquarius::stream_handler<std::shared_ptr<__rpc::tcp_request>, __rpc::tcp_response, int>      \
	{                                                                                                                  \
	public:                                                                                                            \
		using request_type = __rpc::tcp_request;                                                                       \
		using base_type = aquarius::stream_handler<std::shared_ptr<__rpc::tcp_request>, __rpc::tcp_response, int>;     \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__handler_##__rpc))                                                         \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<int> override;                                                    \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<int>

#define AQUARIUS_BROADCAST_HANDLER(method, __response)                                                                 \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::tcp::auto_handler_register<aquarius::tcp::server_session, __response, method>    \
		__auto_register_##method(__response::proto);                                                                   \
	class method : public aquarius::basic_handler<__response, int>                                                     \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::basic_handler<__response, int>;                                                    \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__handler_##__response))                                                    \
		{}                                                                                                             \
	protected:                                                                                                         \
		virtual auto handle() -> aquarius::awaitable<int> override;                                                    \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<int>

#define AQUARIUS_TRANSFER_HANDLER(method, buffer)                                                                      \
	class method : public aquarius::basic_handler<buffer, int>                                                         \
	{                                                                                                                  \
	public:                                                                                                            \
		using request_type = buffer;                                                                                   \
		using base_type = aquarius::basic_handler<buffer, int>;                                                        \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__handler_##buffer))                                                        \
		{}                                                                                                             \
	protected:                                                                                                         \
		virtual auto handle() -> aquarius::awaitable<int> override;                                                    \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<int>