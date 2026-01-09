#include "player_schedule.h"
#include "player_module.h"
#include <aquarius/module/schedule.hpp>

namespace aquarius
{
	namespace gateway
	{
		auto mpc_insert_player(std::size_t id) -> awaitable<void>
		{
			co_return co_await mpc::call<void, player_module>(
				"player_module", [&](player_module* ptr) -> awaitable<void>
				{ co_return ptr->insert(id, std::make_shared<player>(id)); });
		}

		auto mpc_erase_player(std::size_t id) -> awaitable<void>
		{
			co_return co_await mpc::call<void, player_module>(
				"player_module", [&](player_module* ptr) -> awaitable<void> { co_return ptr->erase(id); });
		}

		auto mpc_get_player(std::size_t id) -> awaitable<std::shared_ptr<player>>
		{
			co_return co_await mpc::call<std::shared_ptr<player>, player_module>(
				"player_module",
				[&](player_module* ptr) -> awaitable<std::shared_ptr<player>> { co_return ptr->get(id); });
		}
	} // namespace gateway
} // namespace aquarius