#include "service_center_module.h"
#include "payload.hpp"

namespace aquarius
{
	namespace serviced
	{
		void service_center_module::regist(std::shared_ptr<customer_base> customer_ptr)
		{
			std::unique_lock lk(mutex_);

			auto& group = services_[customer_ptr->group()];

			group.push_back(customer_ptr);
		}

		void service_center_module::remove(std::shared_ptr<customer_base> service_ptr)
		{
			std::unique_lock lk(mutex_);

			services_.erase(service_ptr->group());
		}

		std::shared_ptr<customer_base> service_center_module::get_service(const std::string& group)
		{
			std::shared_lock lk(mutex_);

			auto iter = services_.find(group);

			if (iter == services_.end())
			{
				return nullptr;
			}

			// 负载均衡
			return round_robin().invoke(iter->second);
		}

		auto service_center_module::timer(std::chrono::milliseconds ms) -> asio::awaitable<void>
		{
			for (auto& srvs : services_)
			{
				for (auto& srv : srvs.second)
				{
					asio::co_spawn(
						co_await asio::this_coro::executor, [&] -> asio::awaitable<void> { co_await srv->heart(); },
						asio::detached);
				}
			}
		}
	} // namespace serviced
} // namespace aquarius