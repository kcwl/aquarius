#pragma once
#include <aquarius/basic_handler.hpp>
#include <aquarius/ip/server_session.hpp>
#include <aquarius/module/channel_module.hpp>
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/tcp_request.hpp>

namespace aquarius
{
	template <typename Session, typename Request, typename Response>
	class handler : public basic_handler<Session>
	{
	public:
		using request_t = Request;

		using response_t = Response;

		using session_t = Session;

	public:
		handler(const std::string& name)
			: basic_handler<Session>(name)
		{}

		virtual ~handler() = default;

	public:
		virtual auto visit(std::shared_ptr<Session> sessoin_ptr) -> awaitable<error_code> override
		{
			this->session_ptr_ = sessoin_ptr;

			co_return co_await send_response(co_await this->handle());
		}

		std::shared_ptr<Request> request() const
		{
			return request_ptr_;
		}

		Response& response()
		{
			return response_;
		}

	protected:
		virtual auto handle() -> awaitable<error_code> = 0;

		auto session() const
		{
			return session_ptr_.lock();
		}

	private:
		virtual auto send_response(error_code ec) -> awaitable<error_code>
		{
			response().header().result(ec.value());

			response().seq_number(request()->seq_number());

			flex_buffer buffer{};

			response().commit(buffer);

			if (!this->session())
				co_return error_code{};

			co_return co_await this->session()->async_send(std::move(buffer));
		}

	protected:
		std::shared_ptr<request_t> request_ptr_;

		response_t response_;

		std::weak_ptr<Session> session_ptr_;
	};

	template <typename Session, typename Handler>
	struct auto_handler_register
	{
		explicit auto_handler_register(std::string_view proto)
		{
			mpc_subscribe<Session>(proto, [] { return std::make_shared<Handler>(); });
		}
	};

} // namespace aquarius

#define AQUARIUS_GLOBAL_STR_ID(request) #request

#define __AQUARIUS_HANDLER_IMPL(__session, __method, __request, __response)                                            \
	class __method final : public aquarius::handler<__session, __request, __response>                                  \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::handler<__session, __request, __response>;                                         \
		using session_t = typename base_type::session_t;                                                               \
                                                                                                                       \
	public:                                                                                                            \
		__method()                                                                                                     \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__method))                                                  \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<aquarius::error_code> override;                                   \
	};                                                                                                                 \
	inline auto __method::handle() -> aquarius::awaitable<aquarius::error_code>

#define AQUARIUS_CONTEXT_BY(__session, __request, __response, __method)                                                \
	class __method;                                                                                                    \
	[[maybe_unused]] static aquarius::auto_handler_register<__session, __method> __auto_register_##__method(           \
		__request::this_router);                                                                                       \
	__AQUARIUS_HANDLER_IMPL(__session, __method, __request, __response)

#define AQUARIUS_HANDLER(__request, __response, __method)                                                              \
	using custom_session##__request =                                                                                  \
		aquarius::server_session<typename aquarius::handler_tag_traits<__request>::type,                               \
								 typename aquarius::handler_tag_traits<__request>::selector>;                          \
	AQUARIUS_CONTEXT_BY(custom_session##__request, __request, __response, __method)

#define AQUARIUS_SSL_HANDLER(__request, __response, __method)                                                          \
	using custom_session##__request =                                                                                  \
		aquarius::ssl_server_session<typename aquarius::handler_tag_traits<__request>::type,                           \
									 typename aquarius::handler_tag_traits<__request>::selector>;                      \
	AQUARIUS_CONTEXT_BY(custom_session##__request, __request, __response, __method)