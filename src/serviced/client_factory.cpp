#include "client_factory.h"

namespace serviced
{
	void client_factory::insert(const std::string& ip_addr, std::shared_ptr<client> client_ptr)
	{
		std::lock_guard lk(mutex_);

		auto iter = clients_.find(ip_addr);
		if (iter != clients_.end())
		{
			clients_.erase(ip_addr);
		}

		clients_.insert({ ip_addr, client_ptr });
	}

	std::shared_ptr<client_factory::client> client_factory::find(const std::string& ip_addr)
	{
		std::lock_guard lk(mutex_);

		auto iter = clients_.find(ip_addr);

		if (iter == clients_.end())
			return nullptr;

		return iter->second;
	}

	std::vector<std::shared_ptr<client_factory::client>> client_factory::get_client()
	{
		std::lock_guard lk(mutex_);

		std::vector<std::shared_ptr<client_factory::client>> results{};

		for (auto& m : clients_)
		{
			results.push_back(m.second);
		}

		return results;
	}

	void client_factory::remove(const std::string& ip_addr)
	{
		std::lock_guard lk(mutex_);

		clients_.erase(ip_addr);
	}
}