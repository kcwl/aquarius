#pragma once
#include "channel.h"
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
		public:
			service_center_module() = default;

		public:
			void regist(std::shared_ptr<customer> customer_ptr);

			void remove(std::shared_ptr<customer> customer_ptr);

			auto get_services(const std::string& group, const std::string& topic)
				-> std::expected<std::vector<std::shared_ptr<customer>>, bool>;

			void subscribe(const std::string& group, const std::string& topic,
						   std::shared_ptr<subscriber> subscriber_ptr);

			auto publish(const std::string& group, const std::string& topic, flex_buffer& buffer)
				-> asio::awaitable<error_code>;

		private:
			std::shared_mutex mutex_;

			std::map<std::string, std::shared_ptr<channel>> channel_groups_;
		};
	} // namespace serviced
} // namespace aquarius