#pragma once
#include <aquarius/module/module.hpp>
#include <map>
#include <mutex>

namespace aquarius
{
	class player_base
	{
	public:
		player_base() = default;

		virtual ~player_base() = default;
	};

	class player_module : public no_config_module<player_module>
	{
	public:
		player_module(io_context& io, const std::string& name)
			: no_config_module<player_module>(io, name)
		{}

	public:
		void insert(std::size_t id, std::shared_ptr<player_base> player_ptr)
		{
			std::lock_guard lk(mutex_);

			players_.emplace(id, player_ptr);
		}

		template <typename T>
		std::shared_ptr<T> get(std::size_t id)
		{
			std::lock_guard lk(mutex_);

			auto iter = players_.find(id);
			if (iter == players_.end())
				return nullptr;

			return std::dynamic_pointer_cast<T>(iter->second);
		}

		void erase(std::size_t id)
		{
			std::lock_guard lk(mutex_);

			players_.erase(id);
		}

	private:
		std::mutex mutex_;

		std::map<std::size_t, std::shared_ptr<player_base>> players_;
	};

	inline auto mpc_player_insert(std::size_t id, std::shared_ptr<player_base> player) -> awaitable<void>
	{
		co_return co_await mpc::call<void, player_module>([&](player_module* ptr) mutable -> awaitable<void>
														  { co_return ptr->insert(id, player); });
	}

	template <typename T>
	inline auto mpc_player_erase(std::size_t id) -> awaitable<void>
	{
		co_return co_await mpc::call<void, player_module>([&](player_module* ptr) -> awaitable<void>
														  { co_return ptr->erase(id); });
	}

	template <typename T>
	inline auto mpc_player_get(std::size_t id) -> awaitable<std::shared_ptr<T>>
	{
		co_return co_await mpc::call<std::shared_ptr<T>, player_module>(
			[&](player_module* ptr) -> awaitable<std::shared_ptr<T>> { co_return ptr->get<T>(id); });
	}
} // namespace aquarius