#pragma once
#include "player.h"
#include <aquarius/module/module.hpp>
#include <aquarius/module/module_router.hpp>

namespace aquarius
{
	namespace gateway
	{
		class player_module : public no_config_module<player_module>
		{
		public:
			player_module(const std::string& name);

		public:
			void insert(std::size_t id, std::shared_ptr<player> player_ptr);

			std::shared_ptr<player> get(std::size_t id);

			void erase(std::size_t id);

		private:
			std::mutex mutex_;

			std::map<std::size_t, std::shared_ptr<player>> players_;
		};
	} // namespace gateway
} // namespace aquarius