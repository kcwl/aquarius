#pragma once
#include "gate_error_code.h"
#include "payload.hpp"
#include <aquarius.hpp>
#include <aquarius/tcp.hpp>
#include <proto/regist.virgo.h>

namespace aquarius
{
	namespace gateway
	{
		AQUARIUS_MODULE(client_pool)
		{
			constexpr static std::size_t max_connection = 2;

		public:
			client_pool() = default;

		public:
			virtual bool init() override;

			virtual auto run() -> asio::awaitable<bool> override;

		public:
			template <typename Response, typename Request>
			auto invoke(const std::string& instance, std::shared_ptr<Request> request) -> asio::awaitable<Response>
			{
				std::shared_lock lk(mutex_);

				auto iter = pool_.find(instance);

				if (iter == pool_.end())
				{
					co_return Response{};
				}

				auto ptr = round_.invoke(iter->second);

				// 负载
				co_return co_await ptr->template async_call<Response>(request);
			}

			template <typename Func>
			auto invoke(const std::string& instance, flex_buffer buffer, Func&& func) -> asio::awaitable<error_code>
			{
				std::shared_lock lk(mutex_);

				auto iter = pool_.find(instance);

				if (iter == pool_.end())
				{
					co_return gate_op::not_exist_in_pool;
				}

				auto ptr = round_.invoke(iter->second);

				// 负载
				//co_return co_await ptr->async_send(std::move(buffer), std::forward<Func>(func));
				co_return co_await ptr->async_send(std::move(buffer));
			}

		private:
			auto add(const std::string& host, int32_t port) -> asio::awaitable<void>;

			void remove(const std::string& host, int32_t port);

			auto make_instance_pool(const std::vector<instance>& instances) -> asio::awaitable<void>;

			std::string make_instance(const std::string& host, int32_t port);

			template <typename Response, typename Request>
			auto shake(const std::string& host, int32_t port, std::shared_ptr<Request> request)
				-> asio::awaitable<Response>
			{
				co_await add(host, port);

				co_return co_await invoke<Response>(make_instance(host, port), request);
			}

		private:
			std::shared_mutex mutex_;

			std::map<std::string, std::vector<std::shared_ptr<tcp_client>>> pool_;

			std::string group_;

			round_robin<tcp_client> round_;
		};
	} // namespace gateway
} // namespace aquarius