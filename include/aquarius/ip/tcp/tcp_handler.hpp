#pragma once
#include <aquarius/basic_handler.hpp>
#include <aquarius/ip/tcp/tcp_router.hpp>
#include <aquarius/ip/tcp/tcp_server_session.hpp>

namespace aquarius
{

	template <typename Session, typename Request, typename Response>
	class basic_tcp_hander : public basic_handler<Session, Request, Response>
	{
	public:
		basic_tcp_hander(const std::string& name)
			: basic_handler<Session, Request, Response>(name)
		{}

		virtual ~basic_tcp_hander() = default;

	public:
		virtual auto visit(std::shared_ptr<Session> session_ptr, std::shared_ptr<Request> request_ptr)
			-> awaitable<error_code> override
		{
			this->session_ptr_ = session_ptr;
			this->request_ptr_ = request_ptr;

			auto ec = co_await this->handle();

			co_return co_await this->send_response(ec);
		}

		virtual auto send_response(error_code ec) -> awaitable<error_code>
		{
			this->response_.result(ec.value());

			detail::flex_buffer<char> buffer{};

			this->response_.commit(buffer);

			co_return co_await this->session()->async_send(std::move(buffer));
		}
	};

	template <typename Session, typename Handler, typename Router>
	struct auto_tcp_handler_register
	{
		explicit auto_tcp_handler_register(std::string_view proto)
		{
			Router::get_mutable_instance().regist<Handler>(proto);
		}
	};
} // namespace aquarius

#define __AQUARIUS_TCP_HANDLER(__session, __method, __request, __response)                                             \
	class __method final : public aquarius::basic_tcp_hander<__session, __request, __response>                         \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::basic_tcp_hander<__session, __request, __response>;                                \
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
