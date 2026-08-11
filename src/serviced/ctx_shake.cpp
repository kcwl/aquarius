#include "basic_transfer_context.hpp"
#include "client_pool.h"
#include "proto/shake.virgo.h"
#include <aquarius.hpp>

namespace aquarius
{
	namespace serviced
	{
		AQUARIUS_SYS_HANDLER(shake_request, shake_response, ctx_shake)
		{
			auto result = co_await mpc_async_call<&client_pool::add>(
				request()->body().group(), request()->body().host(), static_cast<uint16_t>(request()->body().port()),
				request()->body().name(), request()->body().weight(), request()->body().version(),
				request()->body().topics());

			if (!result)
			{
				co_return errc::create_client_error;
			}

			auto ctx_func = [host = request()->body().host(), port = static_cast<uint16_t>(request()->body().port()),
							 group = request()->body().group()]<typename Func>(
								flex_buffer& buffer, const std::string& router, Func&& f) -> asio::awaitable<error_code>
			{
				auto tf = [func = std::move(f)](flex_buffer& buf, const std::string& r) -> asio::awaitable<error_code>
				{ co_return co_await func(buf, r); };

				co_return co_await mpc_async_call<&client_pool::invoke_buffer<decltype(tf), flex_buffer&>>(
					group, host, port, std::ref(buffer), router, tf);
			};

			std::shared_ptr<context_base> ctx = std::make_shared<basic_transfer_context<tcp>>(ctx_func);

			for (auto& topic : request()->body().topics())
			{
				XLOG_INFO() << "Register Context " << topic;
				mpc_put_context(topic, ctx, false);
			}

			if (request()->body().group() == "gateway")
			{
				auto& topics = response().body().topics();
				co_await mpc_async_call<&client_pool::generate_topics>(std::ref(topics));
			}

			co_return errc::success;
		}
	} // namespace serviced
} // namespace aquarius