#pragma once
#include "player.h"
#include <aquarius/singleton.hpp>
#include <map>
#include <mutex>

namespace aquarius
{
	namespace gateway
	{
		class player_manager : public singleton<player_manager>
		{
		public:
			void insert(std::size_t id, std::shared_ptr<player> player_ptr);

			std::shared_ptr<player> get(std::size_t id);

			void erase(std::size_t id);

		private:
			std::mutex mutex_;

			std::map<std::size_t, std::shared_ptr<player>> players_;
		};
	} // namespace gateway

	template <typename... Args>
	void insert_player(std::size_t id, Args&&... args)
	{
		return gateway::player_manager::get_mutable_instance().insert(
			std::make_shared<player>(id, std::forward<Args>(args)...));
	}

	std::shared_ptr<gateway::player> defer_player(std::size_t id)
	{
		return gateway::player_manager::get_mutable_instance().get(id);
	}

	void erase_player(std::size_t id)
	{
		return gateway::player_manager::get_mutable_instance().erase(id);
	}
} // namespace aquarius