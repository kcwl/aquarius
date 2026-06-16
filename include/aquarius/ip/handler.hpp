#pragma once
#include <aquarius/concepts.hpp>
#include <aquarius/ip/context_reg.hpp>
#include <aquarius/ip/context_traits.hpp>
#include <expected>

namespace aquarius
{
	template <typename Response>
	class basic_handler
	{
	public:
		using handle_message_t = Response;

		using session_callback = std::function<asio::awaitable<error_code>(flex_buffer&)>;

	public:
		basic_handler(const std::string& name)
			: name_(name)
		{}

	public:
		Response& response()
		{
			return response_;
		}

		std::string name() const
		{
			return name_;
		}

		virtual error_code visit(flex_buffer& buffer)
		{
			return response().consume(buffer);
		}

		void attach_session(const session_callback& cb)
		{
			cb_ = cb;
		}

		session_callback session()
		{
			return cb_;
		}

		virtual auto handle() -> asio::awaitable<error_code> = 0;

	private:
		std::string name_;

		Response response_;

		session_callback cb_;
	};

	template <typename Request, typename Response>
	class handler : public basic_handler<Response>
	{
	public:
		using base = basic_handler<Response>;

		using handle_message_t = Request;

	public:
		handler(const std::string& name)
			: base(name)
			, request_ptr_(std::make_shared<Request>())
		{}

	public:
		virtual error_code visit(flex_buffer& buffer)
		{
			return request()->consume(buffer);
		}

		std::shared_ptr<Request> request() const
		{
			return request_ptr_;
		}

	private:
		std::shared_ptr<Request> request_ptr_;

		std::string name_;
	};

	template <typename Handler>
	struct auto_handler_register
	{
		explicit auto_handler_register(std::string_view proto, bool system = false)
		{
			using context_type = context_traits<typename Handler::handle_message_t, Handler>::type;
			mpc_put_context(std::string(proto), std::make_shared<context_type>(), system);
		}
	};

} // namespace aquarius

#define AQUARIUS_GLOBAL_STR_ID(request) #request

#define __AQUARIUS_HANDLER_IMPL(__handler, __request, __response)                                                      \
	class __handler final : public aquarius::handler<__request, __response>                                            \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::handler<__request, __response>;                                                    \
                                                                                                                       \
	public:                                                                                                            \
		__handler()                                                                                                    \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__handler))                                                 \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::asio::awaitable<aquarius::error_code> override;                             \
	};                                                                                                                 \
	inline auto __handler::handle() -> aquarius::asio::awaitable<aquarius::error_code>

#define __AQUARIUS_BASIC_HANDLER_IMPL(__handler, __response)                                                           \
	class __handler final : public aquarius::basic_handler<__response>                                                 \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::basic_handler<__response>;                                                         \
                                                                                                                       \
	public:                                                                                                            \
		__handler()                                                                                                    \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__handler))                                                 \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::asio::awaitable<aquarius::error_code> override;                             \
	};                                                                                                                 \
	inline auto __handler::handle() -> aquarius::asio::awaitable<aquarius::error_code>

#define AQUARIUS_HANDLER(__request, __response, __handler)                                                             \
	class __handler;                                                                                                   \
	[[maybe_unused]] static aquarius::auto_handler_register<__handler> __auto_register_##__handler(                    \
		__request::this_router);                                                                                       \
	__AQUARIUS_HANDLER_IMPL(__handler, __request, __response)

#define AQUARIUS_SYS_HANDLER(__request, __response, __handler)                                                             \
	class __handler;                                                                                                   \
	[[maybe_unused]] static aquarius::auto_handler_register<__handler> __auto_register_##__handler(                    \
		__request::this_router, true);                                                                                       \
	__AQUARIUS_HANDLER_IMPL(__handler, __request, __response)

#define AQUARIUS_BASIC_HANDLER(__response, __handler)                                                                  \
	class __handler;                                                                                                   \
	[[maybe_unused]] static aquarius::auto_handler_register<__handler> __auto_register_##__handler(                    \
		__response::this_router);                                                                                      \
	__AQUARIUS_BASIC_HANDLER_IMPL(__handler, __response)