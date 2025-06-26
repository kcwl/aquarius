#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/context/handler_router.hpp>
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

		template <typename Session, typename Context>
		struct auto_transfer_handler_register
		{
			explicit auto_transfer_handler_register(std::size_t proto)
			{
				detail::handler_router<Session>::get_mutable_instance().template regist<Context>(proto);
			}
		};
	} // namespace context
} // namespace aquarius

#define AQUARIUS_GLOBAL_ID(request) std::hash<std::string>()(#request)

#define AQUARIUS_GLOBAL_STR_ID(request) #request

#define AQUARIUS_STREAM_HANDLER(method,error, __rpc)                                                                   \
	class method : public aquarius::stream_handler<__rpc::request, __rpc::response, error>                             \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::stream_handler<__rpc::request, __rpc::response, error>;                            \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##method))                                                    \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<error> override;                                                  \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<error>