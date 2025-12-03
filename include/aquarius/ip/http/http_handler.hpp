#pragma once
#include <aquarius/basic_handler.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/ip/http/http_param.hpp>

namespace aquarius
{

	template <typename Session, typename Request, typename Response>
	class http_handler : public basic_handler<Session, Request, Response>
	{
	public:
		http_handler(const std::string& name)
			: basic_handler<Session, Request, Response>(name)
		{}

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

	template <typename Session, typename Context, typename Router>
	struct auto_http_handler_register
	{
		explicit auto_http_handler_register(std::string_view proto)
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
					session->get_executor(), [session, ec, req = std::move(request)]() mutable -> awaitable<void>
					{ co_await std::make_shared<Context>()->visit(session, req, ec != virgo::http_status::ok); }, detached);

				return ec == virgo::http_status::ok;
			};

			Router::get_mutable_instance().regist(proto, func);
		}
	};
} // namespace aquarius

#define __AQUARIUS_HTTP_HANDLER(__session, __method, __request, __response)                                            \
	class __method final : public aquarius::http_handler<__session, __request, __response>                             \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::http_handler<__session, __request, __response>;                                    \
                                                                                                                       \
	public:                                                                                                            \
		__method()                                                                                                     \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__method))                                                  \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<aquarius::error_code> override;                                   \
	};                                                                                                                 \
	inline auto __method::handle() -> aquarius::awaitable<aquarius::error_code>

#define AQUARIUS_CONTEXT_BY_HTTP(__session, __request, __response, __method, __router)                                 \
	class __method;                                                                                                    \
	[[maybe_unused]] static aquarius::auto_http_handler_register<__session, __method, __router>                        \
		__auto_register_##__method(__request::router);                                                                 \
	__AQUARIUS_HTTP_HANDLER(__session, __method, __request, __response)