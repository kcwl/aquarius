#pragma once
#include <aquarius/basic_handler.hpp>
#include <aquarius/ip/server_session.hpp>
#include <aquarius/module/channel_module.hpp>
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/tcp_request.hpp>

namespace aquarius
{
	template <typename Request, typename Response>
	class handler : public basic_handler<Request>
	{
		using base = basic_handler<Request>;

	public:
		using typename base::request_t;

		using response_t = Response;

	public:
		handler(const std::string& name)
			: base(name)
		{}

		virtual ~handler() = default;

	public:
		virtual auto visit(error_code& ec) -> awaitable<flex_buffer> override
		{
			make_response(co_await this->handle());

			flex_buffer buffer{};

			ec = response().commit(buffer);

			co_return std::move(buffer);
		}

		Response& response()
		{
			return response_;
		}

	protected:
		virtual auto handle() -> awaitable<error_code> = 0;

	private:
		void make_response(error_code result)
		{
			response().result(result.value());

			response().header().sequence(this->request()->header().sequence());
		}

	private:
		response_t response_;
	};

	template <typename Handler>
	struct auto_handler_register
	{
		explicit auto_handler_register(std::string_view proto)
		{
			auto f = [] { return std::dynamic_pointer_cast<basic_handler<virgo::http_null_request>>(std::make_shared<Handler>()); };
			mpc_subscribe(proto, f);
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
		virtual auto handle() -> aquarius::awaitable<aquarius::error_code> override;                                   \
	};                                                                                                                 \
	inline auto __handler::handle() -> aquarius::awaitable<aquarius::error_code>

#define AQUARIUS_HANDLER(__request, __response, __handler)                                                             \
	class __handler;                                                                                                   \
	[[maybe_unused]] static aquarius::auto_handler_register<__handler> __auto_register_##__handler(                    \
		__request::this_router);                                                                                       \
	__AQUARIUS_HANDLER_IMPL(__handler, __request, __response)