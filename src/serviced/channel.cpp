#include "channel.h"

namespace aquarius
{
	namespace serviced
	{
		void channel::regist(const std::string& topic, std::shared_ptr<subscriber> subscriber_ptr)
		{
			auto& group = subscribers_[topic];

			group.push_back(subscriber_ptr);
		}

		void channel::regist(std::shared_ptr<customer> customer_ptr)
		{
			auto& group = customers_[customer_ptr->topic()];
			group.push_back(customer_ptr);
		}

		void channel::remove(const std::string& topic, std::shared_ptr<subscriber> subscriber_ptr)
		{
			auto iter = subscribers_.find(topic);
			if (iter == subscribers_.end())
			{
				return;
			}

			auto it = std::find_if(iter->second.begin(), iter->second.end(), [subscriber_ptr](auto susbcriber)
								   { return susbcriber->id() == subscriber_ptr->id(); });

			if (it == iter->second.end())
			{
				return;
			}

			iter->second.erase(it);
		}

		void channel::remove(std::shared_ptr<customer> customer_ptr)
		{
			auto iter = customers_.find(customer_ptr->topic());
			if (iter == customers_.end())
			{
				return;
			}

			subscribers_.erase(customer_ptr->topic());

			customers_.erase(iter);
		}

		auto channel::invoke(const std::string& topic, flex_buffer& buffer) -> asio::awaitable<void>
		{
			auto iter = subscribers_.find(topic);
			if (iter == subscribers_.end())
			{
				co_return;
			}

			for (auto& customer : iter->second)
			{
				flex_buffer_view temp_buf((char*)buffer.data().data(), buffer.data().size());
				co_await mpc_async_call<&session_store_module::invoke>(customer->id(), temp_buf);
			}
		}

		bool channel::empty() const
		{
			return subscribers_.size();
		}

		auto channel::get_services(const std::string& topic)
			-> std::expected<std::vector<std::shared_ptr<customer>>, bool>
		{
			auto iter = customers_.find(topic);

			if (iter == customers_.end())
			{
				return std::unexpected(false);
			}

			return iter->second;
		}
	} // namespace serviced
} // namespace aquarius