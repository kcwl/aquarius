#pragma once
#include <aquarius/basic_handler.hpp>
#include <aquarius/ip/router.hpp>
#include <aquarius/ip/server_session.hpp>
#include <aquarius/virgo/http_fields.hpp>
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
		virtual auto make_response() -> awaitable<void> override
		{
			auto ver = co_await mpc_http_version();
			this->response().version(virgo::from_version_string(ver));

			this->response().set_field("Content-Type", this->request()->find("Content-Type"));
			this->response().set_field("Server", "Aquarius 0.10.0");
			this->response().set_field("Connection", this->request()->find("Connection"));
			this->response().set_field("Access-Control-Allow-Origin", "*");

			this->response().set_field("seq_number", this->request()->find("seq_number"));
		}
	};

	template <typename Session, typename Context>
	struct auto_handler_register
	{
		explicit auto_handler_register(std::string_view proto)
		{
			auto func =
				[&](std::shared_ptr<Session> session, std::shared_ptr<header_field_base> hf, flex_buffer& buffer)
			{
				std::shared_ptr<typename Context::request_t> request;

				if constexpr (Session::tag == proto_tag::http)
				{
					request = std::make_shared<typename Context::request_t>(
						*std::dynamic_pointer_cast<virgo::http_fields>(hf));
				}
				else
				{
					request = std::make_shared<typename Context::request_t>(*hf);
				}

				error_code ec = virgo::http_status::ok;

				try
				{
					request->consume(buffer);
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
	AQUARIUS_CONTEXT_BY(aquarius::server_session<aquarius::handler_tag_traits<__request>::tag>, __request, __response, \
						__method)

#define AQUARIUS_SSL_HANDLER(__request, __response, __method)                                                          \
	AQUARIUS_CONTEXT_BY(aquarius::ssl_server_session<aquarius::handler_tag_traits<__request>::tag>, __request,         \
						__response, __method)