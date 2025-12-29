#pragma once
#include <aquarius/basic_handler.hpp>
#include <aquarius/ip/router.hpp>
#include <aquarius/ip/server_session.hpp>
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
	class handler<proto_tag::http, Session, Request, Response> : public basic_handler<Session, Request, Response>
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
			this->response().version(get_http_param().version);

			this->response().set_field("Content-Type", this->request()->find("Content-Type"));
			this->response().set_field("Server", "Aquarius 0.10.0");
			this->response().set_field("Connection", this->request()->find("Connection"));
			this->response().set_field("Access-Control-Allow-Origin", "*");
		}
	};

	template <auto Tag, typename Session, typename Context>
	struct auto_handler_register
	{
		explicit auto_handler_register(std::string_view proto)
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

			router<Tag, Session>::get_mutable_instance().regist(proto, func);
		}
	};

	template <typename Session, typename Context>
	struct auto_handler_register<proto_tag::http, Session, Context>
	{
		explicit auto_handler_register(std::string_view proto)
		{
			auto func = [&](std::shared_ptr<Session> session, virgo::http_fields hf, flex_buffer& buffer)
			{
				auto request = std::make_shared<typename Context::request_t>();

				error_code ec = virgo::http_status::ok;

				try
				{
					request->consume(buffer);

					request->move_copy(hf);
				}
				catch (error_code& ex)
				{
					ec = ex;
				}

				co_spawn(
					session->get_executor(), [session, ec, request]() mutable -> awaitable<void>
					{ co_await std::make_shared<Context>()->visit(session, request, ec != virgo::http_status::ok); },
					detached);

				return ec == virgo::http_status::ok;
			};

			router<proto_tag::http, Session>::get_mutable_instance().regist(proto, func);
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
	[[maybe_unused]] static aquarius::auto_handler_register<aquarius::handler_tag_traits<__request>::tag, __session,   \
															__method> __auto_register_##__method(__request::router);   \
	__AQUARIUS_HANDLER_IMPL(__session, __method, __request, __response)

#define AQUARIUS_HANDLER(__request, __response, __method)                                                              \
	AQUARIUS_CONTEXT_BY(aquarius::server_session<aquarius::handler_tag_traits<__request>::tag>, __request, __response, \
						__method)

#define AQUARIUS_SSL_HANDLER(__request, __response, __method)                                                          \
	AQUARIUS_CONTEXT_BY(aquarius::ssl_server_session<aquarius::handler_tag_traits<__request>::tag>, __request,         \
						__response, __method)