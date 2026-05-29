#pragma once
#include "customer.h"
#include "proto/regist.virgo.h"
#include "subscriber.h"
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/module/module_register.hpp>
#include <map>
#include <shared_mutex>

namespace aquarius
{
	namespace serviced
	{
		AQUARIUS_MODULE(service_center_module)
		{
			class channel
			{
				friend class service_center_module;

			public:
				channel() = default;

			public:
				auto subscribe(std::shared_ptr<subscriber> subscriber_ptr) -> asio::awaitable<std::vector<uint64_t>>;

				auto publish(std::shared_ptr<customer> customer_ptr) -> asio::awaitable<void>;

				bool empty() const;

				std::shared_ptr<customer> get_service(std::size_t id);

			private:
				void unsubscribe(std::shared_ptr<subscriber> subscriber_ptr);

				auto unpublish(std::shared_ptr<customer> customer_ptr) -> asio::awaitable<void>;

			private:
				std::map<std::size_t, std::shared_ptr<subscriber>> subscribers_;

				std::map<std::size_t, std::weak_ptr<customer>> customers_;
			};

		public:
			service_center_module() = default;

		public:
			auto publish(std::shared_ptr<customer> customer_ptr) -> asio::awaitable<void>;

			auto subscribe(const std::string& group, std::shared_ptr<subscriber> subscriber_ptr)
				-> asio::awaitable<std::vector<uint64_t>>;

		private:
			auto remove(std::shared_ptr<customer> customer_ptr)->asio::awaitable<void>;

		private:
			std::shared_mutex mutex_;

			std::map<std::string, std::shared_ptr<channel>> channel_groups_;

			std::map<std::size_t, std::shared_ptr<customer>> customers_;
		};
	} // namespace serviced
} // namespace aquarius