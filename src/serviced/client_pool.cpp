#include "client_pool.h"
#include "basic_transfer_context.hpp"
#include "proto/shake.virgo.h"

namespace aquarius
{
	namespace serviced
	{
		auto client_pool::add(const std::string& group, const std::string& host, uint16_t port, const std::string& name,
							  int32_t weight, const std::string& version, const std::vector<std::string>& topics)
			-> asio::awaitable<bool>
		{
			auto& g = pool_[group];
			g.push_back({});

			auto& back = g.back();

			if (!back)
			{
				back = std::make_shared<client_info>();
			}

			back->host = host;
			back->port = port;
			back->name = name;
			back->healthy = true;
			back->version = version;
			back->topics = topics;
			back->clients.resize(max_connection);

			for (auto& c : back->clients)
			{
				c = std::make_shared<tcp::client>(co_await asio::this_coro::executor, 30ms);

				c->set_close_func([group, host, port, this](auto) -> asio::awaitable<void>
								  { co_return this->remove(group, host, port); });

				auto ec = co_await c->async_connect(host, static_cast<uint16_t>(port));

				if (ec)
				{
					XLOG_ERROR() << "async connect [" << host << ":" << port << "] failed! " << ec.message();
					co_return false;
				}
			}

			co_return true;
		}

		auto client_pool::generate_topics(std::vector<std::string>& topics) -> asio::awaitable<bool>
		{
			for (auto& [_, clients] : pool_)
			{
				for (auto& c : clients)
				{
					std::copy(c->topics.begin(), c->topics.end(), std::back_inserter(topics));
				}
			}

			co_return !topics.empty();
		}

		void client_pool::remove(const std::string& group, const std::string& host, uint16_t port)
		{
			auto& clients = pool_[group];

			auto iter = std::find_if(clients.begin(), clients.end(),
									 [host, port](auto cli) { return cli->host == host && cli->port == port; });

			if (iter == clients.end())
			{
				return;
			}

			clients.erase(iter);
		}

		std::shared_ptr<tcp::client> client_pool::get_client(const std::string& group,
																		  const std::string& host, uint16_t port)
		{
			auto iter = pool_.find(group);

			if (iter == pool_.end())
			{
				return nullptr;
			}

			if (host.empty() || port == 0)
			{
				auto info_ptr = round_robin<client_info>{}.invoke(iter->second);

				if (!info_ptr)
				{
					return nullptr;
				}

				return round_robin<tcp::client>{}.invoke(info_ptr->clients);
			}

			auto it = std::find_if(iter->second.begin(), iter->second.end(), [&](auto client_ptr)
								   { return client_ptr->host == host && client_ptr->port == port; });

			if (it == iter->second.end())
			{
				return nullptr;
			}

			return round_robin<tcp::client>{}.invoke((*it)->clients);
		}
	} // namespace serviced
} // namespace aquarius