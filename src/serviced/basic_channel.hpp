#pragma once
#include "player.h"
#include <aquarius/asio.hpp>
#include <map>
#include <memory>

namespace aquarius
{
	namespace serviced
	{
		template <typename Policy>
		class basic_channel
		{
		public:
			basic_channel() = default;

		public:
			auto publish(flex_buffer& buffer, std::size_t id, error_code& ec) -> awaitable<flex_buffer>
			{
				co_return co_await policy_.publish(subscribers_, buffer, id, ec);
			}

			void subscribe(std::shared_ptr<player> subscriber)
			{
				subscribers_.emplace(subscriber->id(), subscriber);
			}

		private:
			std::map<std::size_t, std::shared_ptr<player>> subscribers_;

			Policy policy_;
		};
	} // namespace serviced
} // namespace aquarius