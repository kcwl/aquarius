#pragma once
#include "channel.hpp"
#include "player.h"
#include <aquarius/module/module.hpp>

namespace aquarius
{
	namespace serviced
	{
		struct channel_config
		{
			constexpr static auto path = "channel"sv;

			bool enabled;
			int64_t timeout;
		};

		class channel_module : public _module<channel_module, channel_config>
		{
		public:
			channel_module(const std::string& name);

		public:
			void subscribe(std::string_view name, std::shared_ptr<player> subscribe);

			auto publish(std::string_view name, flex_buffer& buffer, std::size_t id, error_code& ec) ->awaitable<flex_buffer>;

		private:
			std::mutex mutex_;

			std::map<std::string_view, std::shared_ptr<channel>> channels_;
		};
	} // namespace serviced
} // namespace aquarius