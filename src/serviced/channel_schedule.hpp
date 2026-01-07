#pragma once
#include "channel_module.h"
#include "player.h"
#include <aquarius/asio.hpp>
#include <aquarius/module/schedule.hpp>
#include <memory>
#include <string_view>

namespace aquarius
{
	namespace serviced
	{
		template <typename R, typename Task>
		inline auto mpc_publish(std::string_view topic, Task&& task) -> awaitable<R>
		{
			co_return co_await mpc::call<R, std::shared_ptr<channel>>("channel_module", std::forward<Task>(task));
		}

		inline auto mpc_subscribe(std::string_view topic, std::shared_ptr<player> role_ptr) -> awaitable<void>
		{
			co_await mpc::call<void, channel>("channel_module"sv,
											  [&](std::shared_ptr<channel> ptr) { ptr->subscribe(role_ptr); });
		}

	} // namespace serviced
} // namespace aquarius