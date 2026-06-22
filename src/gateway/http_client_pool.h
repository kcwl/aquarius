#pragma once
#include "gate_error_code.h"
#include "payload.hpp"
#include <aquarius.hpp>
#include <proto/regist.virgo.h>
#include "global_config.hpp"

namespace aquarius
{
	namespace gateway
	{
		AQUARIUS_MODULE(http_client_pool)
		{
			constexpr static std::size_t max_connection = 2;

		public:
			http_client_pool();

		public:
			virtual bool enable() override
			{
				return global_config::get_const_instance().proto == "http1.1";
			}

			virtual auto run() -> asio::awaitable<bool> override;

		public:
			template <typename Response, typename Request, typename... Args>
			auto invoke(uint64_t host_and_port, std::shared_ptr<Request> request, Args&&... args) -> asio::awaitable<Response>
			{
				std::shared_lock lk(mutex_);

				auto iter = pool_.find(host_and_port);

				if (iter == pool_.end())
				{
					co_return Response{};
				}

				auto ptr = round_.invoke(iter->second);

				co_return co_await ptr->template async_call<Response>(request, std::forward<Args>(args)...);
			}

			template <typename Func, typename ConstBufferSequence, typename... Args>
			auto invoke(uint64_t host_and_port, ConstBufferSequence&& req_buffer,const std::string& router, Func&& f, Args&&... args)
				-> asio::awaitable<error_code>
			{
				std::shared_lock lk(mutex_);

				auto iter = pool_.find(host_and_port);

				if (iter == pool_.end())
				{
					co_return gate_op::not_exist_in_pool;
				}

				auto ptr = round_.invoke(iter->second);

				co_return co_await ptr->async_call_buffer(std::forward<ConstBufferSequence>(req_buffer), router,
														  std::forward<Func>(f), std::forward<Args>(args)...);
			}

			auto shake(uint64_t host_and_port) -> asio::awaitable<void>;

		private:
			auto add(uint64_t host_and_port) -> asio::awaitable<void>;

			void remove(uint64_t host_and_port);

			std::pair<std::string, int32_t> instance_to_host(uint64_t host_and_port);

		private:
			std::shared_mutex mutex_;

			std::map<uint64_t, std::vector<std::shared_ptr<http::client>>> pool_;

			std::string group_;

			round_robin<http::client> round_;
		};
	} // namespace gateway
} // namespace aquarius