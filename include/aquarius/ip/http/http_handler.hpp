#pragma once
#include <aquarius/basic_handler.hpp>
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{

	template <typename Session, typename Request, typename Response>
	class http_hander : public basic_handler<Session, Request, Response>
	{
	public:
		http_hander(const std::string& name)
			: basic_handler<Session, Request, Response>(name)
		{
		}

	public:
		virtual void make_response() override
		{
			this->response().version(this->request()->version());

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
			auto func = [&] (std::shared_ptr<Session> session, virgo::http_fields hf, flex_buffer& buffer)
				{
					auto request = std::make_shared<typename Context::request_t>();

					error_code ec{};

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
						session->get_executor(), [session, ec, req = std::move(request)] () mutable -> awaitable<void>
						{
							co_await std::make_shared<Context>()->visit(session, req, ec);
						}, detached);
				};

			Router::get_mutable_instance().regist(proto, func);
		}
	};
} // namespace aquarius

#define __AQUARIUS_HTTP_HANDLER(__session, __method, __request, __response)                                            \
	class __method final : public aquarius::basic_http_handler<__session, __request, __response>                       \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::basic_http_handler<__session, __request, __response>;                              \
                                                                                                                       \
	public:                                                                                                            \
		__method()                                                                                                     \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__method))                                                  \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<aquarius::error_code> override;                                   \
	};                                                                                                                 \
	inline auto __method::handle() -> aquarius::awaitable<aquarius::error_code>

#define AQUARIUS_CONTEXT_BY_HTTP(__session, __request, __response, __method, __router, __http_method)                  \
	class __method;                                                                                                    \
	[[maybe_unused]] static aquarius::auto_http_handler_register<__session, __method, __router, __http_method>         \
		__auto_register_##__method(__request::router);                                                                 \
	__AQUARIUS_HTTP_HANDLER(__session, __method, __request, __response)