#pragma once
#include "player_module.h"
#include <aquarius/module/schedule.hpp>

namespace aquarius
{
	namespace gateway
	{
		inline auto mpc_insert_player(std::size_t id) -> awaitable<void>
		{
			co_return co_await mpc::call<void, player_module>("player_module",
														   [&](std::shared_ptr<player_module> ptr) -> awaitable<void>
														   {
															   ptr->insert(id, std::make_shared<player>(id));
															   co_return;
														   });
		}

		inline auto mpc_erase_player(std::size_t id) -> awaitable<void>
		{
			co_return co_await mpc::call<void, player_module>("player_module",
														   [&](std::shared_ptr<player_module> ptr) -> awaitable<void>
														   {
															   ptr->erase(id);
															   co_return;
														   });
		}

		template <typename R>
		inline auto mpc_get_player(std::size_t id) -> awaitable<R>
		{
			co_return co_await mpc::call<R, player_module>(
				"player_module", [&](std::shared_ptr<player_module> ptr) -> awaitable<R> { co_return ptr->get(id); });
		}

	} // namespace gateway
} // namespace aquarius