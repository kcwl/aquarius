#include "channel_schedule.h"
#include "channel_module.h"
#include <aquarius/module/schedule.hpp>

namespace aquarius
{
	namespace serviced
	{
		auto mpc_publish(std::string_view topic, flex_buffer& buffer) -> awaitable<void>
		{
			co_return co_await mpc::call<void, channel_module>("channel_module",
															   [&](channel_module* ptr) -> awaitable<void>
															   { co_return ptr->publish(topic, buffer); });
		}

		auto mpc_subscribe(std::string_view topic, std::shared_ptr<player> role_ptr) -> awaitable<void>
		{
			co_await mpc::call<void, channel_module>("channel_module"sv, [&](channel_module* ptr) -> awaitable<void>
													 { co_return ptr->subscribe(topic, role_ptr); });
		}
	} // namespace serviced
} // namespace aquarius