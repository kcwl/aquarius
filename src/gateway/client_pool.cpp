#include "client_pool.h"
#include "basic_transfer_context.hpp"
#include "convert.hpp"
#include "gate_error_code.h"
#include "proto/shake.virgo.h"
#include <srvd_client.hpp>

namespace aquarius
{
	namespace gateway
	{
		client_pool::client_pool()
			: group_("gateway")
		{}

		auto client_pool::run() -> asio::awaitable<bool>
		{
			auto request = std::make_shared<shake_request>();

			auto resp =
				co_await mpc_async_call<&serviced::srvd_client::async_call<shake_response, shake_request>>(request);

			co_await set_topics(resp.body().topics());

			co_return true;
		}

		auto client_pool::set_topics(const std::vector<std::string>& topics) -> asio::awaitable<void>
		{
			auto ctx_func = [this]<typename Func>(flex_buffer& buffer, const std::string& router,
												  Func&& f) -> asio::awaitable<error_code>
			{
				auto tf = [func = std::move(f)](flex_buffer& buf, const std::string&) -> asio::awaitable<error_code>
				{
					flex_buffer out{};
#ifdef HTTP_GATEWAY
					auto ec = convert<tcp, http>::apply(buf, out);
					co_return co_await func(out, ec);
#else
					co_return co_await func(buf);
#endif
				};

				co_return co_await mpc_async_call<
					&serviced::srvd_client::async_call_buffer<decltype(tf), flex_buffer&>>(std::ref(buffer), router, tf);
			};

			std::shared_ptr<context_base> ctx = std::make_shared<basic_transfer_context<
#ifdef HTTP_GATEWAY
				http, http_method
#else
				tcp
#endif
				>>(ctx_func);

			for (auto& topic : topics)
			{
				mpc_put_context(topic, ctx, true);
			}

			co_return;
		}

		std::pair<std::string, int32_t> client_pool::instance_to_host(uint64_t host_and_port)
		{
			uint32_t host = host_and_port >> 32;
			int32_t port = static_cast<int32_t>(host_and_port);

			return { asio::ip::address_v4(host).to_string(), port };
		}
	} // namespace gateway
} // namespace aquarius