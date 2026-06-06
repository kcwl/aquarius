#include "service_center_module.h"
#include "proto/regist.virgo.h"
#include <aquarius.hpp>

namespace
{
	uint64_t make_host_and_port(const std::string& host, int32_t port)
	{
		uint64_t host_u = asio::ip::address(asio::ip::make_address(host)).to_v4().to_uint();

		return (host_u << 32) | port;
	}
}

namespace aquarius
{
	namespace serviced
	{
		auto service_center_module::channel::subscribe(std::shared_ptr<subscriber> subscriber_ptr)
			-> asio::awaitable<std::vector<uint64_t>>
		{
			std::vector<uint64_t> result{};

			auto iter = subscribers_.find(subscriber_ptr->id());

			if (iter != subscribers_.end())
			{
				co_return result;
			}

			subscribers_.insert({ subscriber_ptr->id(), subscriber_ptr });

			for (auto& cter : customers_)
			{
				if (cter.second.expired())
				{
					continue;
				}

				result.push_back({});
				auto& ins = result.back();
				ins = make_host_and_port(cter.second.lock()->host(), cter.second.lock()->port());
			}

			co_return result;
		}

		auto service_center_module::channel::publish(std::shared_ptr<customer> customer_ptr) -> asio::awaitable<void>
		{
			auto iter = customers_.find(customer_ptr->id());

			if (iter != customers_.end())
			{
				co_return;
			}

			customers_.insert({ customer_ptr->id(), customer_ptr });

			for (const auto& sub : subscribers_)
			{
				flex_buffer buf{};
				broad_service_status_tcp_response resp{};
				resp.body().group() = customer_ptr->group();
				resp.body().host_and_port() = make_host_and_port(customer_ptr->host(), customer_ptr->port());
				resp.commit(buf);

				co_await mpc_invoke_session(sub.first, buf, "");
			}
		}

		void service_center_module::channel::unsubscribe(std::shared_ptr<subscriber> subscriber_ptr)
		{
			auto iter = subscribers_.find(subscriber_ptr->id());

			if (iter == subscribers_.end())
			{
				return;
			}

			subscribers_.erase(iter);
		}

		auto service_center_module::channel::unpublish(std::shared_ptr<customer> customer_ptr) -> asio::awaitable<void>
		{
			auto iter = customers_.find(customer_ptr->id());
			if (iter == customers_.end())
			{
				co_return;
			}

			customers_.erase(iter);

			flex_buffer buf{};
			broad_service_status_tcp_response resp{};
			resp.body().host_and_port() = make_host_and_port(customer_ptr->host(), customer_ptr->port());
			resp.body().healty() = false;
			resp.commit(buf);

			 co_await mpc_invoke_session(customer_ptr->id(), buf, "");
		}

		bool service_center_module::channel::empty() const
		{
			return subscribers_.size();
		}

		auto service_center_module::publish(std::shared_ptr<customer> customer_ptr) -> asio::awaitable<void>
		{
			std::unique_lock lk(mutex_);

			auto iter = channel_groups_.find(customer_ptr->group());

			if (iter != channel_groups_.end())
			{
				co_await iter->second->publish(customer_ptr);
			}
			else
			{
				auto& ptr = channel_groups_[customer_ptr->group()];
				if (!ptr)
				{
					ptr = std::make_shared<channel>();
				}
				co_await ptr->publish(customer_ptr);
			}

			customers_.insert({ customer_ptr->id(), customer_ptr });
		}

		auto service_center_module::subscribe(const std::string& group, std::shared_ptr<subscriber> subscriber_ptr)
			-> asio::awaitable<std::vector<uint64_t>>
		{
			std::unique_lock lk(mutex_);

			auto& chan = channel_groups_[group];

			if (!chan)
			{
				chan = std::make_shared<channel>();
			}

			co_return co_await chan->subscribe(subscriber_ptr);
		}

		auto service_center_module::remove(std::shared_ptr<customer> customer_ptr) -> asio::awaitable<void>
		{
			std::unique_lock lk(mutex_);

			customers_.erase(customer_ptr->id());

			auto iter = channel_groups_.find(customer_ptr->group());

			if (iter == channel_groups_.end())
			{
				co_return;
			}

			co_await iter->second->unpublish(customer_ptr);

			if (iter->second->empty())
			{
				channel_groups_.erase(iter);
			}
		}
	} // namespace serviced
} // namespace aquarius