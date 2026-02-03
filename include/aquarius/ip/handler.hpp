#pragma once
#include <aquarius/basic_handler.hpp>
#include <aquarius/ip/router.hpp>
#include <aquarius/ip/server_session.hpp>
#include <aquarius/virgo/header_fields.hpp>
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/tcp_request.hpp>

namespace aquarius
{
	template <auto Tag, typename Session, typename Request, typename Response>
	class handler : public basic_handler<Session, Request, Response>
	{
	public:
		using session_t = Session;

	public:
		handler(const std::string& name)
			: basic_handler<Session, Request, Response>(name)
		{}

		virtual ~handler() = default;
	};

	template <typename Session, typename Request, typename Response>
	class handler<proto::http, Session, Request, Response> : public basic_handler<Session, Request, Response>
	{
	public:
		using session_t = Session;

	public:
		handler(const std::string& name)
			: basic_handler<Session, Request, Response>(name)
		{}

		virtual ~handler() = default;

	public:
		virtual void make_response() override
		{
			this->response().set_field("Content-Type", "application/json");
			this->response().set_field("Server", "Aquarius 0.10.0");
			this->response().set_field("Connection", this->request()->find("Connection"));
			this->response().set_field("Access-Control-Allow-Origin", "*");
			this->response().set_field("Access-Control-Request-Method", "POST");
			this->response().set_field("Access-Control-Expose-Headers", "Aquarius-Header");
		}
	};

	template <typename Session, typename Context>
	struct auto_handler_register
	{
		explicit auto_handler_register(std::string_view proto)
		{
			auto func = [&](std::shared_ptr<Session> session, virgo::header_fields hf, flex_buffer& buffer)
			{
				std::shared_ptr<typename Context::request_t> request;

				request = std::make_shared<typename Context::request_t>();

				error_code ec{};

				try
				{
					request->set_header_fields(std::move(hf));
					request->consume(buffer);
				}
				catch (error_code& ex)
				{
					ec = ex;
				}
				catch (...)
				{
					ec = virgo::http_status::bad_request;
				}

				co_spawn(
					session->get_executor(),
					[session, ec, request]() mutable -> awaitable<void>
					{
						co_await std::make_shared<Context>()->visit(session, request, ec);
					},
					detached);

				return !ec;
			};

			router<Session>::get_mutable_instance().regist(proto, func);
		}
	};

} // namespace aquarius

#define AQUARIUS_GLOBAL_STR_ID(request) #request

#define __AQUARIUS_HANDLER_IMPL(__session, __method, __request, __response)                                            \
	class __method final                                                                                               \
		: public aquarius::handler<aquarius::handler_tag_traits<__request>::tag, __session, __request, __response>     \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type =                                                                                              \
			aquarius::handler<aquarius::handler_tag_traits<__request>::tag, __session, __request, __response>;         \
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
		__request::router);                                                                                            \
	__AQUARIUS_HANDLER_IMPL(__session, __method, __request, __response)

#define AQUARIUS_HANDLER(__request, __response, __method)                                                              \
	using custom_session = aquarius::server_session<aquarius::handler_tag_traits<__request>::tag,                      \
													aquarius::handler_tag_traits<__request>::selector>;                \
	AQUARIUS_CONTEXT_BY(custom_session, __request, __response, __method)

#define AQUARIUS_SSL_HANDLER(__request, __response, __method)                                                          \
	AQUARIUS_CONTEXT_BY(aquarius::ssl_server_session<aquarius::handler_tag_traits<__request>::tag>, __request,         \
						__response, __method)