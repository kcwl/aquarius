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

			virtual auto run(io_service_pool& pool) -> asio::awaitable<bool> override;

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

			auto invoke(const std::string& instance, flex_buffer& buffer) -> asio::awaitable<error_code>
			{
				std::shared_lock lk(mutex_);

				auto iter = pool_.find(instance);

				if (iter == pool_.end())
				{
					co_return gate_op::not_exist_in_pool;
				}

				auto ptr = round_.invoke(iter->second);

				co_return co_await ptr->async_send(buffer);
			}

			auto make_one_instance(const instance& c) ->asio::awaitable<void>;

			auto make_one_instance_by_host(const std::string& host, int32_t port) ->asio::awaitable<void>;

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

			std::map<std::string, std::vector<std::shared_ptr<tcp::client>>> pool_;

			std::string group_;

			round_robin<tcp::client> round_;
		};
	} // namespace gateway
} // namespace aquarius