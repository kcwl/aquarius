#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/context/detail/handler_router.hpp>
#include <aquarius/context/stream_handler.hpp>

namespace aquarius
{
	namespace context
	{
		template <typename Session, typename RPC, typename Context>
		struct auto_handler_register
		{
			explicit auto_handler_register(std::size_t proto)
			{
				detail::handler_router<Session>::get_mutable_instance().template regist<typename RPC::request, Context>(proto);
			}
		};
	} // namespace context
} // namespace aquarius

#define AQUARIUS_GLOBAL_ID(request) std::hash<std::string>()(#request)

#define AQUARIUS_GLOBAL_STR_ID(request) #request

#define AQUARIUS_STREAM_HANDLER(method, __rpc)                                                                         \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::context::auto_handler_register<                                                  \
		aquarius::tcp::server_session, __rpc, method> __auto_register_##method(AQUARIUS_GLOBAL_ID(__rpc));    \
	class method : public aquarius::stream_handler<__rpc::request, __rpc::response, int>              \
	{                                                                                                                  \
	public:                                                                                                                  \
		using base_type = aquarius::stream_handler<__rpc::request, __rpc::response, int>;             \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__rpc))                                                         \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<int> override;                                                    \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<int>

#define AQUARIUS_BROADCAST_HANDLER(method, __rpc)                                                                      \
	class method;                                                                                                      \
	[[maybe_unused]] static aquarius::context::auto_handler_register<                                                  \
		aquarius::tcp::server_session, __rpc, method> __auto_register_##method(AQUARIUS_GLOBAL_ID(__rpc));  \
	class method : public aquarius::basic_handler<__rpc, int>                                                \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::basic_handler<__rpc, int>;                                               \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__rpc))                                                         \
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
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##buffer))                                                        \
		{}                                                                                                             \
	protected:                                                                                                         \
		virtual auto handle() -> aquarius::awaitable<int> override;                                                    \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<int>