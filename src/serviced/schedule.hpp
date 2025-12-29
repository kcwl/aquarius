#pragma once
#include "channel_role.hpp"
#include "channel_task.hpp"
#include <aquarius/asio.hpp>
#include <aquarius/module/schedule.hpp>
#include <string_view>
#include "channel_group.hpp"
#include "channel.hpp"

namespace aquarius
{
	namespace serviced
	{
		template <typename Session>
		auto schedule_subscribe(std::string_view topic, std::shared_ptr<channel_role<Session>> role) ->awaitable<void>
		{
			co_await module_schedule<channel_group<channel<Session>>>("channelmodule", std::make_shared<subscribe_task<channel_role<Session>>>(topic, role));
		}

		template <typename Session, typename T>
		auto schedule_publish(std::string_view topic, T&& v) -> awaitable<void>
		{
			co_await module_schedule<channel_group<channel<Session>>>("channelmodule", std::make_shared<publish_task<T>>(topic, std::forward<T>(v)));
		}
	} // namespace serviced
} // namespace aquarius
