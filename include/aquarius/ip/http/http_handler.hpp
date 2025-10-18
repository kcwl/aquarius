#pragma once
#include <aquarius/basic_handler.hpp>

namespace aquarius
{

	template <typename Session, typename Request, typename Response>
	class basic_http_hander : public basic_handler<Session, Request, Response>
	{
	public:
		basic_http_hander(const std::string& name)
			: basic_handler<Session, Request, Response>(name)
		{}

	public:
		virtual auto visit(std::shared_ptr<Session> session_ptr, std::shared_ptr<Request> request_ptr)
			-> awaitable<error_code> override
		{
			this->session_ptr_ = session_ptr;
			this->request_ptr_ = request_ptr;

			this->response_.version(this->request_ptr_->version());

			this->response_.set_field("Content-Type", this->request_ptr_->find("Content-Type"));
			this->response_.set_field("Server", "Aquarius 0.10.0");
			this->response_.set_field("Connection", this->request_ptr_->find("Connection"));
			this->response_.set_field("Access-Control-Allow-Origin", "*");

			auto ec = co_await this->handle();

			co_return co_await this->send_response(ec);
		}

		virtual auto send_response(error_code ec) -> awaitable<error_code>
		{
			this->response_.result(ec.value());

			this->response_.reason(virgo::from_status_string(ec.value()));

			detail::flex_buffer<char> buffer{};

			this->response_.commit(buffer, ec);

			if (ec.value() != static_cast<int>(virgo::http_status::ok))
				co_return ec;

			co_return co_await this->session()->async_send(std::move(buffer));
		}
	};

	template <typename Session, typename Handler, typename Router>
	struct auto_http_handler_register
	{
		explicit auto_http_handler_register(virgo::http_method method, std::string_view proto)
		{
			Router::get_mutable_instance().template regist<Handler>(method, proto);
		}
	};
} // namespace aquarius

#define __AQUARIUS_HTTP_HANDLER(__session, __method, __request, __response)                                            \
	class __method final : public aquarius::basic_http_hander<__session, __request, __response>                        \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::basic_http_hander<__session, __request, __response>;                               \
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
	[[maybe_unused]] static aquarius::auto_http_handler_register<__session, __method, __router>                        \
		__auto_register_##__method(__http_method, __request::router);                                                  \
	__AQUARIUS_HTTP_HANDLER(__session, __method, __request, __response)

#define AQUARIUS_HTTP_POST_HANDLER(__request, __response, __method)                                                    \
	AQUARIUS_CONTEXT_BY_HTTP(aquarius::http_server_session, __request, __response, __method,                           \
							 aquarius::http_router<aquarius::http_server_session>, aquarius::virgo::http_method::post)

#define AQUARIUS_HTTP_GET_HANDLER(__request, __response, __method)                                                     \
	AQUARIUS_CONTEXT_BY_HTTP(aquarius::http_server_session, __request, __response, __method,                           \
							 aquarius::http_router<aquarius::http_server_session>, aquarius::virgo::http_method::get)

#define AQUARIUS_HTTP_OPTIONS_HANDLER(__request, __response, __method)                                                 \
	AQUARIUS_CONTEXT_BY_HTTP(aquarius::http_server_session, __request, __response, __method,                           \
							 aquarius::http_router<aquarius::http_server_session>,                                     \
							 aquarius::virgo::http_method::options)