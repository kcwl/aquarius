#include "client_factory.h"

namespace serviced
{
	void client_factory::insert(std::size_t topic, const std::string& host, const std::string& port, std::shared_ptr<client> client_ptr)
	{
		std::lock_guard lk(mutex_);

		auto& cli_list = clients_[topic];

		auto iter = std::find_if(cli_list.begin(), cli_list.end(), [&](auto client_ptr)
			{
				if (!client_ptr)
					return false;

				return client_ptr->remote_address() == host && std::to_string(client_ptr->remote_port()) == port;
			});

		if (iter != cli_list.end())
			cli_list.erase(iter);
		

		cli_list.push_back(client_ptr);
	}

	std::shared_ptr<client_factory::client> client_factory::find(std::size_t topic, const std::string& host, const std::string& port)
	{
		std::lock_guard lk(mutex_);

		auto iter = clients_.find(topic);

		if (iter == clients_.end())
			return nullptr;

		auto& cli_list = iter->second;

		auto it = std::find_if(cli_list.begin(), cli_list.end(), [&](auto client_ptr)
			{
				if (!client_ptr)
					return false;

				return client_ptr->remote_address() == host && std::to_string(client_ptr->remote_port()) == port;
			});

		return  it != cli_list.end() ? *it : nullptr;
	}

	std::vector<std::shared_ptr<client_factory::client>> client_factory::get_client()
	{
		std::lock_guard lk(mutex_);

		std::vector<std::shared_ptr<client_factory::client>> results{};

		for (auto& m : clients_)
		{
			for(auto& c : m.second)

			results.push_back(c);
		}

		return results;
	}

	void client_factory::remove(std::size_t topic, const std::string& host, const std::string& port)
	{
		std::lock_guard lk(mutex_);

		auto iter = clients_.find(topic);

		if (iter == clients_.end())
			return;

		auto& cl_list = iter->second;

		auto it = std::find_if(cl_list.begin(), cl_list.end(), [&](auto client_ptr)
			{
				if (!client_ptr)
					return false;

				return client_ptr->remote_address() == host && std::to_string(client_ptr->remote_port()) == port;
			});

		if (it == cl_list.end())
			return;

		cl_list.erase(it);
	}
}