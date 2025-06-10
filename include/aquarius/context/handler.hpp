#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/context/detail/handler_router.hpp>
#include <aquarius/context/stream_handler.hpp>
#include <aquarius/ip/tcp.hpp>

namespace aquarius
{
	namespace context
	{
		template <typename Session, typename Request, typename Context>
		struct auto_handler_register
		{
			explicit auto_handler_register(std::size_t proto)
			{
				detail::handler_router<Session>::get_mutable_instance().template regist<Request, Context>(proto);
			}
		};
	} // namespace context
} // namespace aquarius

#define AQUARIUS_GLOBAL_ID(request) #request

#define AQUARIUS_DEFAULT_HANDLER(method, __request)                                                                    \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::context::auto_handler_register<                                                  \
		aquarius::ip::tcp::server_session, __request, method> __auto_register_##method(__request::proto);              \
	class method : public aquarius::basic_handler<__request, int>                                                      \
	{                                                                                                                  \
	public:                                                                                                            \
		using request_type = __request;                                                                                \
		using base_type = aquarius::basic_handler<__request, int>;                                                     \
                                                                                                                       \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__handler_##__request))                                                     \
		{}                                                                                                             \
                                                                                                                       \
	protected:                                                                                                         \
		virtual auto handle() -> aquarius::awaitable<int> override;                                                    \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<int>

#define AQUARIUS_STREAM_HANDLER(method, __rpc)                                                                         \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::context::auto_handler_register<aquarius::ip::tcp::server_session,                \
																	 __rpc::tcp_request, method>                       \
		__auto_register_##method(__rpc::tcp_request::proto);                                                           \
	class method : public aquarius::stream_handler<std::shared_ptr<__rpc::tcp_request>, __rpc::tcp_response, int>      \
	{                                                                                                                  \
	public:                                                                                                            \
		using request_type = __rpc::tcp_request;                                                                       \
		using base_type = aquarius::stream_handler<std::shared_ptr<__rpc::tcp_request>, __rpc::tcp_response, int>;     \
                                                                                                                       \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__handler_##__rpc))                                                         \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<int> override;                                                    \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<int>

#define AQUARIUS_BROADCAST_HANDLER(method, __response)                                                                 \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::context::auto_handler_register<                                                  \
		aquarius::ip::tcp::server_session, __response, method> __auto_register_##method(__response::proto);            \
	class method : public aquarius::basic_handler<__response, int>                                                     \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::basic_handler<__response, int>;                                                    \
                                                                                                                       \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__handler_##__response))                                                    \
		{}                                                                                                             \
                                                                                                                       \
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
                                                                                                                       \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_ID(__handler_##buffer))                                                        \
		{}                                                                                                             \
                                                                                                                       \
	protected:                                                                                                         \
		virtual auto handle() -> aquarius::awaitable<int> override;                                                    \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<int>