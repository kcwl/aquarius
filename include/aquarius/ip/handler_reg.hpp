#pragma once
#include <aquarius/ip/context_reg.hpp>
#include <aquarius/ip/context_traits.hpp>
#include <aquarius/ip/handler.hpp>

namespace aquarius
{
	template <typename Handler>
	struct auto_handler_register
	{
		explicit auto_handler_register(std::string_view proto, bool system = false)
		{
			using context_type = context_traits<typename Handler::handle_message_t, Handler>::type;
			mpc_put_context(std::string(proto), std::make_shared<context_type>(), system);
		}
	};
} // namespace aquarius

#define AQUARIUS_GLOBAL_STR_ID(request) #request

#define __AQUARIUS_HANDLER_IMPL(__handler, __request, __response)                                                      \
	class __handler final : public aquarius::handler<__request, __response>                                            \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::handler<__request, __response>;                                                    \
                                                                                                                       \
	public:                                                                                                            \
		__handler()                                                                                                    \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__handler))                                                 \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::asio::awaitable<aquarius::error_code> override;                             \
	};                                                                                                                 \
	inline auto __handler::handle() -> aquarius::asio::awaitable<aquarius::error_code>

#define __AQUARIUS_BASIC_HANDLER_IMPL(__handler, __response)                                                           \
	class __handler final : public aquarius::basic_handler<__response>                                                 \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::basic_handler<__response>;                                                         \
                                                                                                                       \
	public:                                                                                                            \
		__handler()                                                                                                    \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__handler))                                                 \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::asio::awaitable<aquarius::error_code> override;                             \
	};                                                                                                                 \
	inline auto __handler::handle() -> aquarius::asio::awaitable<aquarius::error_code>

#define AQUARIUS_HANDLER(__request, __response, __handler)                                                             \
	class __handler;                                                                                                   \
	[[maybe_unused]] static aquarius::auto_handler_register<__handler> __auto_register_##__handler(                    \
		__request::this_router);                                                                                       \
	__AQUARIUS_HANDLER_IMPL(__handler, __request, __response)

#define AQUARIUS_SYS_HANDLER(__request, __response, __handler)                                                         \
	class __handler;                                                                                                   \
	[[maybe_unused]] static aquarius::auto_handler_register<__handler> __auto_register_##__handler(                    \
		__request::this_router, true);                                                                                 \
	__AQUARIUS_HANDLER_IMPL(__handler, __request, __response)

#define AQUARIUS_BASIC_HANDLER(__response, __handler)                                                                  \
	class __handler;                                                                                                   \
	[[maybe_unused]] static aquarius::auto_handler_register<__handler> __auto_register_##__handler(                    \
		__response::this_router);                                                                                      \
	__AQUARIUS_BASIC_HANDLER_IMPL(__handler, __response)