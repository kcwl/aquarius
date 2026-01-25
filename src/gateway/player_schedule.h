#pragma once
#include <aquarius/asio.hpp>
#include "player.h"

namespace aquarius
{
	namespace gateway
	{
		auto mpc_insert_player(std::size_t id) -> awaitable<void>;

		auto mpc_erase_player(std::size_t id) -> awaitable<void>;

		auto mpc_get_player(std::size_t id) -> awaitable<std::shared_ptr<player>>;

	} // namespace gateway
} // namespace aquarius