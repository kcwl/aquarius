#include "customer.h"
#include "error.hpp"
#include "proto/channel.virgo.h"
#include "service_center_module.h"
#include "session_handler.hpp"

namespace aquarius
{
	namespace serviced
	{
		AQUARIUS_SYS_SERVICED_HANDLER(regist_request, regist_response, ctx_regist)
		{
			auto customer_ptr = std::make_shared<customer>(detail::uuid_generator()());

			customer_ptr->name(request()->body().name());
			customer_ptr->host(request()->body().host());
			customer_ptr->port(request()->body().port());
			customer_ptr->healthy(request()->body().healthy());
			customer_ptr->group(request()->body().group());
			customer_ptr->weight(request()->body().weight());
			customer_ptr->version(request()->body().version());

			customer_ptr->attach_session(this->session());

			co_await mpc_async_call<&service_center_module::publish>(customer_ptr);

			co_return errc::success;
		}

		AQUARIUS_SYS_SERVICED_HANDLER(subscribe_service_request, subscribe_service_response, ctx_subscribe_service)
		{
			auto subscriber_ptr = std::make_shared<subscriber>(detail::uuid_generator()());

			subscriber_ptr->attach_session(this->session());

			response().body().instances() =
				co_await mpc_async_call<&service_center_module::subscribe>(request()->body().group(), subscriber_ptr);

			co_return errc::success;
		}

	} // namespace serviced
} // namespace aquarius