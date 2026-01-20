#pragma once
#include "player.h"
#include <aquarius/asio.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <memory>
#include <string_view>

namespace aquarius
{
	namespace serviced
	{
		auto mpc_publish(std::string_view topic, flex_buffer& task) -> awaitable<flex_buffer>;

		auto mpc_subscribe(std::string_view topic, std::shared_ptr<player> role_ptr) -> awaitable<void>;

	} // namespace serviced
} // namespace aquarius