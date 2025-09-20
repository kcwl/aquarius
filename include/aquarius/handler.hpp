#pragma once
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/router.hpp>
#include <aquarius/awaitable.hpp>
#include <aquarius/executor/http_server_session.hpp>
#include <aquarius/executor/tcp_server_session.hpp>
#include <aquarius/error_code.hpp>
// #include <aquarius/detail/handler.hpp>
//
// #define AQUARIUS_CONTEXT_BY(__session, method, __rpc) \
//	class method;                                                                                                      \
//	[[maybe_unused]] static aquarius::detail::auto_handler_register<__session, __rpc, method>                          \
//		__auto_register_##method(std::to_string(__rpc::router()));                                                     \
//	__AQUARIUS_TCP_HANDLER(__session, method, __rpc::request, __rpc::response)
//
// #define AQUARIUS_TCP_HANDLER(method, __rpc) AQUARIUS_CONTEXT_BY(aquarius::tcp_server_session, method, __rpc)
//
// #define AQUARIUS_RAW_CONTEXT() \
// 	AQUARIUS_CONTEXT_BY(aquarius::tcp_server_session, transfer_handle, \
// aquarius::rpc_transfer_flow) \
//
// #define AQUARIUS_TCP_RESPONSE(method, __rpc) \
//	class method;                                                                                                      \
//	[[maybe_unused]] static aquarius::detail::auto_handler_register<aquarius::tcp_client_session, __rpc, method>       \
//		__auto_register_##method(std::to_string(__rpc::router()));                                                     \
//	__AQUARIUS_TCP_HANDLER(aquarius::tcp_client_session, method, __rpc::response)
//
// #define AQUARIUS_HTTP_CONTEXT_BY(__session, method, __rpc) \
//	class method;                                                                                                      \
//	[[maybe_unused]] static aquarius::detail::auto_handler_register<__session, __rpc, method>                          \
//		__auto_register_##method(std::string(__rpc::router().data(), __rpc::router().size()));                        \
//	__AQUARIUS_HTTP_HANDLER(__session, method, __rpc::request, __rpc::response)
//
// #define AQUARIUS_HTTP_HANDLER(method, __rpc) AQUARIUS_HTTP_CONTEXT_BY(aquarius::http_server_session, method, __rpc)
//
//

namespace aquarius
{
	template <typename Session, typename Request, typename Response>
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
		auto visit(std::shared_ptr<Session> sessoin_ptr, std::shared_ptr<request_t> request_ptr)
			-> awaitable<error_code>
		{
			session_ptr_ = sessoin_ptr;
			request_ptr_ = request_ptr;

			auto ec = co_await this->handle();

			co_return co_await this->send_response(ec);
		}
		std::string name() const
		{
			return name_;
		}

	protected:
		virtual auto handle() -> awaitable<error_code> = 0;

		std::shared_ptr<Request> request() const
		{
			return request_ptr_;
		}

		Response& response()
		{
			return response_;
		}

		auto session()
		{
			return session_ptr_;
		}

		virtual auto send_response(error_code ec) -> awaitable<error_code>
		{
			response_.result(ec.value());

			detail::flex_buffer<char> buffer{};

			response_.commit(buffer, ec);

			if (ec)
				co_return ec;

			co_return co_await session()->async_send(std::move(buffer));
		}

	private:
		std::string name_;

		std::shared_ptr<Session> session_ptr_;

		std::shared_ptr<request_t> request_ptr_;

		response_t response_;
	};

	template <typename Session, typename Handler>
	struct auto_handler_register
	{
		explicit auto_handler_register(std::string_view proto)
		{
			detail::router<Session>::get_mutable_instance().template regist<Handler>(proto);
		}
	};
} // namespace aquarius

#define AQUARIUS_GLOBAL_STR_ID(request) #request

#define __AQUARIUS_HANDLER(__session, __method, __request, __response)                                                 \
	class __method final : public aquarius::basic_handler<__session, __request, __response>                            \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::basic_handler<__session, __request, __response>;                                   \
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
	__AQUARIUS_HANDLER(__session, __method, __request, __response)

#define AQUARIUS_TCP_HANDLER(__request, __response, __method)                                                          \
	AQUARIUS_CONTEXT_BY(aquarius::tcp_server_session, __request, __response, __method)

#define AQUARIUS_HTTP_HANDLER(__request, __response, __method)                                                         \
	AQUARIUS_CONTEXT_BY(aquarius::http_server_session, __request, __response, __method)