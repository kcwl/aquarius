#pragma once
#include <aquarius/channel/publisher.hpp>
#include <aquarius/channel/subscriber.hpp>
#include <aquarius/channel/topic.hpp>

namespace aquarius
{
	using global_watcher = watcher<channel_topic, std::function<void()>>;

	using scope_subscriber = subscriber<global_watcher>;

	using global_subscriber = subscriber<global_watcher, subscriber_global>;

	using global_publisher = publisher<global_watcher>;
}