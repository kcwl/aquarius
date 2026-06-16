#pragma once
#include <aquarius/ip/handler.hpp>

namespace aquarius
{
	namespace serviced
	{
		template <typename Request, typename Response>
		class session_handler : public handler<Request, Response>
		{
		public:
			using base_type = handler<Request, Response>;

			using session_callback = std::function<asio::awaitable<error_code>(flex_buffer&)>;

			using handle_message_t = typename base_type::handle_message_t;

		public:
			session_handler(const std::string& name)
				: handler<Request, Response>(name)
			{}

		public:
			void attach_session(const session_callback& cb)
			{
				cb_ = cb;
			}

			session_callback session()
			{
				return cb_;
			}

		private:
			session_callback cb_;
		};
	} // namespace serviced
} // namespace aquarius

#define __AQUARIUS_SERVICED_HANDLER_IMPL(__handler, __request, __response)                                             \
	class __handler final : public aquarius::serviced::session_handler<__request, __response>                          \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::serviced::session_handler<__request, __response>;                                  \
                                                                                                                       \
	public:                                                                                                            \
		__handler()                                                                                                    \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__handler))                                                 \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::asio::awaitable<aquarius::error_code> override;                             \
	};                                                                                                                 \
	inline auto __handler::handle() -> aquarius::asio::awaitable<aquarius::error_code>

#define AQUARIUS_SERVICED_HANDLER(__request, __response, __handler)                                                    \
	class __handler;                                                                                                   \
	[[maybe_unused]] static aquarius::auto_handler_register<__handler> __auto_register_##__handler(                    \
		__request::this_router);                                                                                       \
	__AQUARIUS_SERVICED_HANDLER_IMPL(__handler, __request, __response)

#define AQUARIUS_SYS_SERVICED_HANDLER(__request, __response, __handler)                                                \
	class __handler;                                                                                                   \
	[[maybe_unused]] static aquarius::auto_handler_register<__handler> __auto_register_##__handler(                    \
		__request::this_router, true);                                                                                 \
	__AQUARIUS_SERVICED_HANDLER_IMPL(__handler, __request, __response)