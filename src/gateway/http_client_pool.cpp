#include "http_client_pool.h"
#include "basic_transfer_context.hpp"
#include "gate_error_code.h"
#include "proto/shake.virgo.h"
#include <srvd_client.hpp>

namespace aquarius
{
	namespace gateway
	{
		http_client_pool::http_client_pool()
			: group_("services")
		{}

		auto http_client_pool::run() -> asio::awaitable<bool>
		{
			auto healthy_callback = [this](uint64_t host_and_port, bool healthy) -> asio::awaitable<void>
			{
				if (!healthy)
				{
					this->remove(host_and_port);
				}
				else
				{
					co_await add(host_and_port);
				}
			};

			co_await mpc_async_call<&serviced::srvd_client::set_healty_check<decltype(healthy_callback)>>(
				healthy_callback);

			co_await mpc_async_call<&serviced::srvd_client::subscribe>(
				group_, std::move(
							[&](const std::vector<uint64_t>& instances) -> asio::awaitable<void>
							{
								for (auto& i : instances)
								{
									co_await this->add(i);
								}
							}));

			co_return true;
		}

		auto http_client_pool::shake(uint64_t host_and_port) -> asio::awaitable<void>
		{
			co_await add(host_and_port);

			auto request = std::make_shared<shake_request>();

			auto resp = co_await this->invoke<shake_response>(host_and_port, request, http_method::post, http_version::http1_1);

			auto ctx_func = [host_and_port, this]<typename Func>(flex_buffer& buffer, const std::string& router,
																 Func&& f) -> asio::awaitable<error_code>
			{
				auto ec = co_await this->invoke(
					host_and_port, buffer, router,
					[func = std::move(f)](flex_buffer& buf, const std::string&) -> asio::awaitable<error_code>
					{ co_return co_await func(buf, 0); },http_method::post, http_version::http1_1);

				co_return ec;
			};

			std::shared_ptr<context_base> ctx =
				std::make_shared<basic_transfer_context<decltype(ctx_func), http>>(std::move(ctx_func));

			for (auto& topic : resp.body().topics())
			{
				mpc_put_context(topic, ctx, true);
			}
		}

		auto http_client_pool::add(uint64_t host_and_port) -> asio::awaitable<void>
		{
			std::unique_lock lk(mutex_);

			auto iter = pool_.find(host_and_port);

			if (iter != pool_.end())
			{
				co_return;
			}

			pool_.insert({ host_and_port, {} });

			auto& back = pool_[host_and_port];

			back.resize(max_connection);

			auto [host, port] = instance_to_host(host_and_port);

			for (auto& c : back)
			{
				c = std::make_shared<http::client>(co_await asio::this_coro::executor, 30ms);

				auto ec = co_await c->async_connect(host, static_cast<uint16_t>(port));

				if (ec)
				{
					co_return;
				}
			}
		}

		void http_client_pool::remove(uint64_t host_and_port)
		{
			std::unique_lock lk(mutex_);

			auto iter = pool_.find(host_and_port);

			if (iter == pool_.end())
			{
				return;
			}

			pool_.erase(iter);
		}

		std::pair<std::string, int32_t> http_client_pool::instance_to_host(uint64_t host_and_port)
		{
			uint32_t host = host_and_port >> 32;
			int32_t port = static_cast<int32_t>(host_and_port);

			return { asio::ip::address_v4(host).to_string(), port };
		}
	} // namespace gateway
} // namespace aquarius