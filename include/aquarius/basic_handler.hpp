#pragma once
#include <aquarius/detail/config.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/coroutine.hpp>
#include <memory>

namespace aquarius
{
	template <typename Request, typename Response>
	class basic_handler
	{
	public:
		using request_t = Request;
		using response_t = Response;

	public:
		basic_handler(const std::string& name)
			: name_(name)
		{}
		virtual ~basic_handler() = default;

	public:
		virtual auto visit(std::shared_ptr<Request> request_ptr) -> awaitable<Response>
		{
			request_ptr_ = request_ptr;

			auto ec = co_await this->handle();

			response().result(ec.value());

			co_return response();
		}

		std::string name() const
		{
			return name_;
		}

		std::shared_ptr<Request> request()
		{
			return request_ptr_;
		}

		Response& response()
		{
			return response_;
		}

	protected:
		virtual auto handle() -> awaitable<error_code> = 0;

	protected:
		std::shared_ptr<request_t> request_ptr_;

		response_t response_;

	private:
		std::string name_;
	};

	template <typename Handler, typename Router>
	struct auto_tcp_handler_register
	{
		explicit auto_tcp_handler_register(std::string_view proto)
		{
			Router::get_mutable_instance().template regist<Handler>(proto);
		}
	};

	template <typename Handler, typename Router, virgo::http_method Method>
	struct auto_http_handler_register
	{
		explicit auto_http_handler_register(std::string_view proto)
		{
			Router::get_mutable_instance().template regist<Method, Handler>(proto);
		}
	};
} // namespace aquarius

#define AQUARIUS_GLOBAL_STR_ID(request) #request

#define __AQUARIUS_TCP_HANDLER(__method, __request, __response)                                                        \
	class __method final : public aquarius::basic_handler<__request, __response>                                        \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::basic_handler<__request, __response>;                                               \
                                                                                                                       \
	public:                                                                                                            \
		__method()                                                                                                     \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__method))                                                  \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<aquarius::error_code> override;                                   \
	};                                                                                                                 \
	inline auto __method::handle() -> aquarius::awaitable<aquarius::error_code>

#define AQUARIUS_CONTEXT_BY_TCP(__request, __response, __method, __router)                                             \
	class __method;                                                                                                    \
	[[maybe_unused]] static aquarius::auto_tcp_handler_register<__method, __router> __auto_register_##__method(        \
		__request::router);                                                                                            \
	__AQUARIUS_TCP_HANDLER(__method, __request, __response)

#define AQUARIUS_TCP_HANDLER(__request, __response, __method)                                                          \
	AQUARIUS_CONTEXT_BY_TCP(__request, __response, __method, aquarius::tcp_router<aquarius::tcp_server_session>)

#define __AQUARIUS_HTTP_HANDLER(__method, __request, __response)                                        \
	class __method final : public aquarius::basic_handler<__request, __response>                                        \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::basic_handler<__request, __response>;                                              \
                                                                                                                       \
	public:                                                                                                            \
		__method()                                                                                                     \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__method))                                                  \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<aquarius::error_code> override;                                   \
	};                                                                                                                 \
	inline auto __method::handle() -> aquarius::awaitable<aquarius::error_code>

#define AQUARIUS_CONTEXT_BY_HTTP(__request, __response, __method, __router, __http_method)                             \
	class __method;                                                                                                    \
	[[maybe_unused]] static aquarius::auto_http_handler_register<__method, __router, __http_method>                    \
		__auto_register_##__method(__request::router);                                                                 \
	__AQUARIUS_HTTP_HANDLER(__method, __request, __response)