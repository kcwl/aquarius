#include "client_pool.h"
#include "proto/shake.virgo.h"
#include <srvd_client.hpp>
#include "gate_error_code.h"

namespace aquarius
{
	CONFIG_MICRO(srv_config, cfg)
	{
		cfg.host = "127.0.0.1";
		cfg.port = 3399;
	}

	namespace gateway
	{
		bool client_pool::init()
		{
			group_ = "gateway";
		}

		auto client_pool::run() -> asio::awaitable<bool>
		{
			mpc_call<&serviced::srvd_client::set_healty_check>(
				[&](const std::string& host, int32_t port, bool healthy) -> asio::awaitable<void>
				{ !healthy ? this->remove(host, port) : co_await this->add(host, port); });

			co_await mpc_async_call<&serviced::srvd_client::subscribe>(
				group_, [&](const std::vector<instance>& instances) -> asio::awaitable<void>
				{ co_await this->make_instance_pool(instances); });

			co_return true;
		}

		auto client_pool::add(const std::string& host, int32_t port) -> asio::awaitable<void>
		{
			std::unique_lock lk(mutex_);

			auto instance = make_instance(host, port);

			auto iter = pool_.find(instance);

			if (iter != pool_.end())
			{
				co_return;
			}

			iter->second.resize(max_connection);

			for (auto& c : iter->second)
			{
				c = std::make_shared<tcp_client>();

				auto ec = co_await c->async_connect(host, static_cast<uint16_t>(port));

				if (ec)
				{
					co_return;
				}
			}
		}

		void client_pool::remove(const std::string& host, int32_t port)
		{
			std::unique_lock lk(mutex_);

			auto instance = make_instance(host, port);

			auto iter = pool_.find(instance);

			if (iter == pool_.end())
			{
				return;
			}

			pool_.erase(iter);
		}

		auto client_pool::make_instance_pool(const std::vector<instance>& instances) -> asio::awaitable<void>
		{
			for (auto& c : instances)
			{
				flex_buffer req_buffer{};
				auto instance = make_instance(c.host, c.port);

				auto request = std::make_shared<shake_tcp_request>();

				auto resp = co_await shake<shake_tcp_response>(c.host, c.port, request);

				auto f = [instance = std::move(instance),
						  this](flex_buffer& buffer, std::size_t session_id,
								int method) -> asio::awaitable<std::expected<flex_buffer, error_code>>
				{
					invoke(instance, std::move(buffer), [session_id](flex_buffer resp) -> asio::awaitable<void>
						   { co_await mpc_async_call<&session_store_module::invoke>(session_id, std::move(resp)); });

					co_return std::unexpected(gate_op::pedding);
				};

				for (auto& topic : resp.body().topics())
				{
					mpc_subscribe(topic, f);
				}
			}
		}

		std::string client_pool::make_instance(const std::string& host, int32_t port)
		{
			return host + ":" + std::to_string(port);
		}
	} // namespace gateway
} // namespace aquarius