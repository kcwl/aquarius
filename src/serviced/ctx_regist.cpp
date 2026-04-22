#include "customer.hpp"
#include "error.hpp"
#include "proto/channel.virgo.h"
#include "proto/regist.virgo.h"
#include "service_center_module.h"
#include <aquarius.hpp>

namespace aquarius
{
	namespace serviced
	{
		AQUARIUS_HANDLER(regist_tcp_request, regist_tcp_response, ctx_regist)
		{
			auto customer_ptr = std::make_shared<customer<tcp_client>>();

			customer_ptr->name(request()->body().name());
			customer_ptr->host(request()->body().host());
			customer_ptr->port(request()->body().port());
			customer_ptr->healthy(request()->body().healthy());
			customer_ptr->group(request()->body().group());
			customer_ptr->weight(request()->body().weight());
			customer_ptr->version(request()->body().version());

			mpc_call<&service_center_module::regist>(customer_ptr);

			auto res = co_await customer_ptr->create_client(co_await asio::this_coro::executor);

			if (!res)
			{
				co_return errc::create_client_error;
			}

			auto req = std::make_shared<subs_list_tcp_request>();

			auto resp = co_await customer_ptr->async_call<subs_list_tcp_response>(req);

			for (auto& topic : resp.body().keys())
			{
				auto f = [&, this, customer_ptr] (flex_buffer& buffer,
												  int32_t method) -> asio::awaitable<std::expected<flex_buffer, error_code>>
					{
						auto srv = mpc_call<&service_center_module::get_service>(customer_ptr->group());
						if (!srv)
						{
							co_return std::unexpected(error_code{});
						}

						auto func = srv->subscribe();

						co_return co_await func(buffer, method);
					};

				mpc_subscribe(customer_ptr->group(), f);
			}

			co_return errc::success;
		}
	} // namespace serviced
} // namespace aquarius