#pragma once
#include <aquarius/module/module.hpp>
#include <map>
#include <mutex>

namespace aquarius
{
	template <typename T>
	class player_module : public no_config_module<player_module<T>>
	{
	public:
		player_module(const std::string& name)
			: no_config_module<player_module<T>>(name)
		{}

	public:
		void insert(std::size_t id, std::shared_ptr<T> player_ptr)
		{
			std::lock_guard lk(mutex_);

			players_.emplace(id, player_ptr);
		}

		std::shared_ptr<T> get(std::size_t id)
		{
			std::lock_guard lk(mutex_);

			auto iter = players_.find(id);
			if (iter == players_.end())
				return nullptr;

			return iter->second;
		}

		void erase(std::size_t id)
		{
			std::lock_guard lk(mutex_);

			players_.erase(id);
		}

	private:
		std::mutex mutex_;

		std::map<std::size_t, std::shared_ptr<T>> players_;
	};
} // namespace aquarius