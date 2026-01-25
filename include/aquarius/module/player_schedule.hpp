#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/module/player_module.hpp>

namespace aquarius
{
	inline auto mpc_player_insert(std::string_view module_name, std::size_t id, std::shared_ptr<player_base> player)
		-> awaitable<void>
	{
		co_return co_await mpc::call<void, player_module>(module_name, [&](player_module* ptr) mutable-> awaitable<void>
														  { co_return ptr->insert(id, player); });
	}

	template <typename T>
	inline auto mpc_player_erase(std::string_view module_name, std::size_t id) -> awaitable<void>
	{
		co_return co_await mpc::call<void, player_module>(module_name, [&](player_module* ptr) -> awaitable<void>
														  { co_return ptr->erase(id); });
	}

	template <typename T>
	inline auto mpc_player_get(std::string_view module_name, std::size_t id) -> awaitable<std::shared_ptr<T>>
	{
		co_return co_await mpc::call<std::shared_ptr<T>, player_module>(
			module_name, [&](player_module* ptr) -> awaitable<std::shared_ptr<T>> { co_return ptr->get<T>(id); });
	}
} // namespace aquarius