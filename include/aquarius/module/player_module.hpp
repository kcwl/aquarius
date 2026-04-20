#pragma once
#include <aquarius/basic_module.hpp>
#include <aquarius/module/module_register.hpp>
#include <memory>
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

	AQUARIUS_MODULE(player_module)
	{
	public:
		player_module() = default;

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
} // namespace aquarius