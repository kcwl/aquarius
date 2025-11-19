#pragma once
#include <aquarius/basic_handler.hpp>
#include <aquarius/ip/tcp/tcp_server_session.hpp>

namespace aquarius
{
	template <typename Session, typename Request, typename Response>
	using tcp_handler = basic_handler<Session, Request, Response>;

	template <typename Session, typename Context, typename Router>
	struct auto_tcp_handler_register
	{
		explicit auto_tcp_handler_register(std::string_view proto)
		{
			auto func = [&](std::shared_ptr<Session> session, flex_buffer& buffer)
			{
				error_code ec{};

				auto req = std::make_shared<typename Context::request_t>();

				try
				{
					req->consume(buffer);
				}
				catch (error_code& ex)
				{
					ec = ex;
				}

				co_spawn(
					session->get_executor(), [session, &ec, request = std::move(req)]() mutable -> awaitable<void>
					{ co_await std::make_shared<Context>()->visit(session, request, !!ec); }, detached);

				return !ec;
			};

			Router::get_mutable_instance().regist(proto, func);
		}
	};

} // namespace aquarius

#define __AQUARIUS_TCP_HANDLER(__session, __method, __request, __response)                                             \
	class __method final : public aquarius::tcp_handler<__session, __request, __response>                               \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::tcp_handler<__session, __request, __response>;                                      \
                                                                                                                       \
	public:                                                                                                            \
		__method()                                                                                                     \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__method))                                                  \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<aquarius::error_code> override;                                   \
	};                                                                                                                 \
	inline auto __method::handle() -> aquarius::awaitable<aquarius::error_code>

#define AQUARIUS_CONTEXT_BY_TCP(__session, __request, __response, __method, __router)                                  \
	class __method;                                                                                                    \
	[[maybe_unused]] static aquarius::auto_tcp_handler_register<__session, __method, __router>                         \
		__auto_register_##__method(__request::router);                                                                 \
	__AQUARIUS_TCP_HANDLER(__session, __method, __request, __response)

#define AQUARIUS_TCP_HANDLER(__request, __response, __method)                                                          \
	AQUARIUS_CONTEXT_BY_TCP(aquarius::tcp_server_session, __request, __response, __method,                             \
							aquarius::tcp_router<aquarius::tcp_server_session>)