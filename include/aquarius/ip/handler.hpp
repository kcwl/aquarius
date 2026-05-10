#pragma once
#include <aquarius/concepts.hpp>
#include <aquarius/ip/context_traits.hpp>
#include <aquarius/module/handler_channel.hpp>
#include <expected>

namespace aquarius
{
	template <typename Response>
	class basic_handler
	{
	public:
		using request_t = Response;
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

		virtual auto visit(flex_buffer& buffer) -> asio::awaitable<error_code>
		{
			response().consume(buffer);

			co_return co_await this->handle();
		}

	protected:
		virtual auto handle() -> asio::awaitable<error_code> = 0;

	private:
		std::string name_;

		Response response_;
	};

	template <typename Request, typename Response>
	class handler : public basic_handler<Response>
	{
	public:
		using base = basic_handler<Response>;

		using request_t = Request;

	public:
		handler(const std::string& name)
			: base(name)
			, request_ptr_(std::make_shared<Request>())
		{}

	public:
		virtual auto visit(flex_buffer& buffer) -> asio::awaitable<error_code>
		{
			request()->consume(buffer);

			co_return co_await this->handle();
		}

		std::shared_ptr<request_t> request() const
		{
			return request_ptr_;
		}

	private:
		void make_response(error_code result)
		{
			this->response().result(result.value());
		}

	private:
		std::shared_ptr<request_t> request_ptr_;

		std::string name_;
	};

	template <typename Handler>
	struct auto_handler_register
	{
		explicit auto_handler_register(std::string_view proto)
		{
			using context_type = context_traits<typename Handler::request_t, Handler>::type;
			mpc_subscribe(std::string(proto), std::make_shared<context_type>());
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

#define AQUARIUS_BASIC_HANDLER(__response, __handler)                                                                  \
	class __handler;                                                                                                   \
	[[maybe_unused]] static aquarius::auto_handler_register<__handler> __auto_register_##__handler(                    \
		__response::this_router);                                                                                      \
	__AQUARIUS_BASIC_HANDLER_IMPL(__handler, __response)