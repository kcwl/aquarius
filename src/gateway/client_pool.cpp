#include "client_pool.h"
#include "basic_transfer_context.hpp"
#include "gate_error_code.h"
#include "proto/shake.virgo.h"
#include <srvd_client.hpp>

namespace aquarius
{
	namespace gateway
	{
		bool client_pool::init()
		{
			group_ = "gateway";

			return true;
		}

		auto client_pool::run() -> asio::awaitable<bool>
		{
			// auto healthy_callback = [this](const std::string& host, int32_t port, bool healthy)
			//{
			//	if (!healthy)
			//	{
			//		this->remove(host, port);
			//	}
			//	else
			//	{
			//		asio::co_spawn(
			//			get_executor(), [this, host, port]() -> asio::awaitable<void>
			//			{ co_await this->add(host, port); }, asio::detached);
			//	}
			// };

			// co_await
			// mpc_call<&serviced::srvd_client::set_healty_check<decltype(healthy_callback)>>(healthy_callback);

			co_await mpc_async_call<&serviced::srvd_client::subscribe>(
				group_, std::move([&](const std::vector<instance>& instances) -> asio::awaitable<void>
								  { co_await this->make_instance_pool(instances); }));

			co_return true;
		}

		auto client_pool::make_one_instance(const instance& c) -> asio::awaitable<void>
		{
			co_return co_await make_one_instance_by_host(c.host, c.port);
		}

		auto client_pool::make_one_instance_by_host(const std::string& host, int32_t port) -> asio::awaitable<void>
		{
			flex_buffer req_buffer{};
			auto inst = make_instance(host, port);

			auto request = std::make_shared<shake_tcp_request>();

			auto resp = co_await shake<shake_tcp_response>(host, port, request);

			auto f = [inst = std::move(inst), this](std::size_t session_id, flex_buffer& buffer) -> asio::awaitable<error_code>
			{
				// buffer.pubseekpos(0, std::ios::in);
				// raw_header* header = (raw_header*)buffer.data().data();
				// regist_resp_func(
				//	header->src, [session_id](flex_buffer resp) -> asio::awaitable<void>
				//	{ co_await mpc_async_call<&session_store_module::invoke>(session_id, std::move(resp)); });
				co_await this->invoke(inst, buffer);

				co_return gate_op::success;
			};

			std::shared_ptr<context_base> ctx =
				std::make_shared<basic_transfer_context<decltype(f), tcp>>(std::move(f));

			for (auto& topic : resp.body().topics())
			{
				mpc_put_context(topic, ctx);
			}
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
				c = std::make_shared<tcp::client>(co_await asio::this_coro::executor, 30ms);

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
				co_await make_one_instance(c);
			}
		}

		std::string client_pool::make_instance(const std::string& host, int32_t port)
		{
			return host + ":" + std::to_string(port);
		}
	} // namespace gateway
} // namespace aquarius