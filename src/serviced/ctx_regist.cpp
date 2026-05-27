#include "customer.h"
#include "error.hpp"
#include "proto/channel.virgo.h"
#include "service_center_module.h"
#include <aquarius.hpp>

namespace aquarius
{
	namespace serviced
	{
		AQUARIUS_HANDLER(regist_tcp_request, regist_tcp_response, ctx_regist)
		{
			auto customer_ptr = std::make_shared<customer>(this->session());

			customer_ptr->name(request()->body().name());
			customer_ptr->host(request()->body().host());
			customer_ptr->port(request()->body().port());
			customer_ptr->healthy(request()->body().healthy());
			customer_ptr->group(request()->body().group());
			customer_ptr->weight(request()->body().weight());
			customer_ptr->version(request()->body().version());

			co_await mpc_async_call<&service_center_module::publish>(customer_ptr);

			co_return errc::success;
		}

		AQUARIUS_HANDLER(subscribe_service_tcp_request, subscribe_service_tcp_response, ctx_subscribe_service)
		{
			auto subscriber_ptr = std::make_shared<subscriber>(this->session());

			response().body().instances() = co_await mpc_async_call<&service_center_module::subscribe>(request()->body().group(), subscriber_ptr);

			co_return errc::success;
		}

	} // namespace serviced
} // namespace aquarius