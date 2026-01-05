#include "player_manager.h"

namespace aquarius
{
	namespace gateway
	{
		void player_manager::insert(std::size_t id, std::shared_ptr<player> player_ptr)
		{
			std::lock_guard lk(mutex_);

			players_.emplace(id, player_ptr);
		}

		std::shared_ptr<player> player_manager::get(std::size_t id)
		{
			std::lock_guard lk(mutex_);

			auto iter = players_.find(id);
			if (iter == players_.end())
				return nullptr;

			return iter->second;
		}

		void player_manager::erase(std::size_t id)
		{
			std::lock_guard lk(mutex_);

			players_.erase(id);
		}
	} // namespace gateway
} // namespace aquarius