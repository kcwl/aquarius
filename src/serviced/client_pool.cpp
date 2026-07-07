#include "client_pool.h"
#include "basic_transfer_context.hpp"
#include "proto/shake.virgo.h"

namespace aquarius
{
	namespace serviced
	{
		auto client_pool::shake(const std::string& group, uint64_t host_and_port, const std::string& name, bool healthy,
								int32_t weight, const std::string& version) -> asio::awaitable<void>
		{
			std::unique_lock lk(mutex_);

			co_await add(group, host_and_port, name, healthy, weight, version);

			auto request = std::make_shared<shake_request>();

			auto resp = co_await this->invoke<shake_response>(group, host_and_port, request);

			auto ctx_func = [host_and_port, this, group]<typename Func>(flex_buffer& buffer, const std::string& router,
																 Func&& f) -> asio::awaitable<error_code>
			{
				co_return co_await this->invoke(
					group, host_and_port, buffer, router,
					[func = std::move(f)](flex_buffer& buf, const std::string&) -> asio::awaitable<error_code>
					{
						co_return co_await func(buf);
					});
			};

			std::shared_ptr<context_base> ctx = std::make_shared<basic_transfer_context<tcp>>(ctx_func);

			for (auto& topic : resp.body().topics())
			{
				mpc_put_context(topic, ctx, true);
			}
		}

		void client_pool::remove(const std::string& group, uint64_t host_and_port)
		{
			std::unique_lock lk(mutex_);

			auto iter = pool_.find(group);

			if (iter == pool_.end())
			{
				return;
			}

			auto it = std::find_if(iter->second.begin(), iter->second.end(),
								   [&](auto client_ptr) { return client_ptr->host_port == host_and_port; });

			if (it == iter->second.end())
			{
				return;
			}

			iter->second.erase(it);
		}

		auto client_pool::add(const std::string& group, uint64_t host_and_port, const std::string& name, bool healthy,
							  int32_t weight, const std::string& version) -> asio::awaitable<void>
		{
			std::unique_lock lk(mutex_);

			auto& g = pool_[group];
			g.push_back({});

			auto& back = g.back();

			back->host_port = host_and_port;
			back->name = name;
			back->healthy = healthy;
			back->version = version;
			back->clients.resize(max_connection);

			auto [host, port] = instance_to_host(host_and_port);

			for (auto& c : back->clients)
			{
				c = std::make_shared<tcp::client>(co_await asio::this_coro::executor, 30ms);

				auto ec = co_await c->async_connect(host, static_cast<uint16_t>(port));

				if (ec)
				{
					co_return;
				}
			}
		}

		std::pair<std::string, int32_t> client_pool::instance_to_host(uint64_t host_and_port)
		{
			uint32_t host = host_and_port >> 32;
			int32_t port = static_cast<int32_t>(host_and_port);

			return { asio::ip::address_v4(host).to_string(), port };
		}
	} // namespace serviced
} // namespace aquarius