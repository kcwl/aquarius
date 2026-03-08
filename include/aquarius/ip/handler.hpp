#pragma once
#include <aquarius/ip/server_session.hpp>
#include <aquarius/module/channel_module.hpp>
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/tcp_request.hpp>
#include <expected>

namespace aquarius
{
	template <typename Request, typename Response>
	class handler
	{
	public:
		using request_t = Request;
		using response_t = Response;

	public:
		handler(const std::string& name)
			: request_ptr_(new Request)
			, name_(name)
		{
		}

	public:
		virtual auto visit(flex_buffer& buffer, int version, error_code& ec) -> awaitable<flex_buffer>
		{
			request()->consume(buffer, version);

			make_response(co_await this->handle());

			flex_buffer resp_buffer{};

			ec = response().commit(resp_buffer);

			co_return std::move(resp_buffer);
		}

		Response& response()
		{
			return response_;
		}

		std::shared_ptr<request_t> request() const
		{
			return request_ptr_;
		}

		std::string name() const
		{
			return name_;
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
		std::shared_ptr<request_t> request_ptr_;

		std::string name_;

		response_t response_;
	};

	template <typename Handler>
	struct auto_handler_register
	{
		explicit auto_handler_register(std::string_view proto)
		{
			auto f = [](flex_buffer& buffer, int version)->aquarius::awaitable<std::expected<flex_buffer,error_code>>
				{ 
					auto handler_ptr =  std::make_shared<Handler>();

					error_code ec{};

					auto resp_buffer = co_await handler_ptr->visit(buffer, version, ec);

					if (ec)
					{
						co_return std::unexpected(ec);
					}

					co_return resp_buffer;
				};
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