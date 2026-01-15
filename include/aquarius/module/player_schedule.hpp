#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/module/player_module.hpp>

namespace aquarius
{
	template <typename T>
	inline auto mpc_player_insert(std::string_view module_name, std::size_t id, std::shared_ptr<T> player)
		-> awaitable<std::shared_ptr<T>>
	{
		co_return co_await mpc::call<std::shared_ptr<T>, player_module<T>>(
			module_name, [&](player_module<T>* ptr) -> awaitable<std::shared_ptr<T>>
			{ co_return co_await ptr->insert(id, player); });
	}

	template <typename T>
	inline auto mpc_player_erase(std::string_view module_name, std::size_t id) -> awaitable<std::shared_ptr<T>>
	{
		co_return co_await mpc::call<std::shared_ptr<T>, player_module<T>>(
			module_name,
			[&](player_module<T>* ptr) -> awaitable<std::shared_ptr<T>> { co_return co_await ptr->erase(id); });
	}

	template <typename T>
	inline auto mpc_player_get(std::string_view module_name, std::size_t id) -> awaitable<std::shared_ptr<T>>
	{
		co_return co_await mpc::call<std::shared_ptr<T>, player_module<T>>(
			module_name,
			[&](player_module<T>* ptr) -> awaitable<std::shared_ptr<T>> { co_return co_await ptr->get(id); });
	}
} // namespace aquarius