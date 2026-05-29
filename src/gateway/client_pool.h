#pragma once
#include "gate_error_code.h"
#include "payload.hpp"
#include <aquarius.hpp>
#include <proto/regist.virgo.h>

namespace aquarius
{
	namespace gateway
	{
		AQUARIUS_MODULE(client_pool)
		{
			constexpr static std::size_t max_connection = 2;

		public:
			client_pool();

		public:
			virtual auto run() -> asio::awaitable<bool> override;

		public:
			template <typename Response, typename Request>
			auto invoke(uint64_t host_and_port, std::shared_ptr<Request> request) -> asio::awaitable<Response>
			{
				std::shared_lock lk(mutex_);

				auto iter = pool_.find(host_and_port);

				if (iter == pool_.end())
				{
					co_return Response{};
				}

				auto ptr = round_.invoke(iter->second);

				co_return co_await ptr->template async_call<Response>(request);
			}

			template<typename Func>
			auto invoke(uint64_t host_and_port, flex_buffer& req_buffer, Func&& f)
				-> asio::awaitable<error_code>
			{
				std::shared_lock lk(mutex_);

				auto iter = pool_.find(host_and_port);

				if (iter == pool_.end())
				{
					co_return gate_op::not_exist_in_pool;
				}

				auto ptr = round_.invoke(iter->second);

				co_return co_await ptr->async_call_buffer(req_buffer, std::forward<Func>(f));
			}

			auto shake(uint64_t host_and_port) -> asio::awaitable<void>;

		private:
			auto add(uint64_t host_and_port) -> asio::awaitable<void>;

			void remove(uint64_t host_and_port);

			std::pair<std::string, int32_t> instance_to_host(uint64_t host_and_port);

		private:
			std::shared_mutex mutex_;

			std::map<uint64_t, std::vector<std::shared_ptr<tcp::client>>> pool_;

			std::string group_;

			round_robin<tcp::client> round_;
		};
	} // namespace gateway
} // namespace aquarius