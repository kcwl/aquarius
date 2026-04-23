#include "service_center_module.h"

namespace aquarius
{
	namespace serviced
	{
		void service_center_module::regist(std::shared_ptr<customer> customer_ptr)
		{
			std::unique_lock lk(mutex_);

			auto iter = channel_groups_.find(customer_ptr->group());

			if (iter != channel_groups_.end())
			{
				iter->second->regist(customer_ptr);
			}
			else
			{
				channel_groups_[customer_ptr->group()]->regist(customer_ptr);
			}
		}

		void service_center_module::remove(std::shared_ptr<customer> customer_ptr)
		{
			std::unique_lock lk(mutex_);

			auto iter = channel_groups_.find(customer_ptr->group());

			if (iter == channel_groups_.end())
			{
				return;
			}

			iter->second->remove(customer_ptr);

			if (iter->second->empty())
			{
				channel_groups_.erase(iter);
			}
		}

		auto service_center_module::get_services(const std::string& group, const std::string& topic)
			-> std::expected<std::vector<std::shared_ptr<customer>>, bool>
		{
			auto iter = channel_groups_.find(group);

			if (iter == channel_groups_.end())
			{
				return std::unexpected(false);
			}

			return iter->second->get_services(topic);
		}

		void service_center_module::subscribe(const std::string& group, const std::string& topic,
											  std::shared_ptr<subscriber> subscriber_ptr)
		{
			std::unique_lock lk(mutex_);

			auto iter = channel_groups_.find(group);

			if (iter == channel_groups_.end())
			{
				channel_groups_[group]->regist(topic, subscriber_ptr);
			}
			else
			{
				iter->second->regist(topic, subscriber_ptr);
			}
		}

		auto service_center_module::publish(const std::string& group, const std::string& topic, flex_buffer& buffer)
			-> asio::awaitable<error_code>
		{
			std::unique_lock lk(mutex_);

			auto iter = channel_groups_.find(group);

			if (iter == channel_groups_.end())
			{
				co_await channel_groups_[group]->invoke(topic, buffer);
			}
			else
			{
				co_await iter->second->invoke(topic, buffer);
			}

			co_return error_code{};
		}
	} // namespace serviced
} // namespace aquarius