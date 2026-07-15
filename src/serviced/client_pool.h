#pragma once
#include "error.hpp"
#include "payload.hpp"
#include <aquarius.hpp>

namespace aquarius
{
	namespace serviced
	{
		AQUARIUS_MODULE(client_pool)
		{
			constexpr static std::size_t max_connection = 2;

			struct client_info
			{
				uint64_t host_port;
				std::string name;
				bool healthy;
				int32_t weight;
				std::string version;
				std::vector<std::shared_ptr<tcp::client>> clients;
			};

		public:
			template <typename Response, typename Request>
			auto invoke(const std::string& group, uint64_t host_and_port, std::shared_ptr<Request> request)
				-> asio::awaitable<Response>
			{
				std::shared_lock lk(mutex_);

				auto iter = pool_.find(group);

				if (iter != pool_.end())
				{
					if (host_and_port == 0)
					{
						auto info_ptr = round_robin<client_info>{}.invoke(iter->second);

						if (info_ptr)
						{
							auto ptr = round_robin<tcp::client>{}.invoke(info_ptr->clients);

							co_return co_await ptr->template async_call<Response>(request);
						}
					}
					else
					{
						auto it = std::find_if(iter->second.begin(), iter->second.end(),
											   [&](auto client_ptr) { return client_ptr->host_port == host_and_port; });

						if (it != iter->second.end())
						{
							auto ptr = round_robin<tcp::client>{}.invoke((*it)->clients);

							co_return co_await ptr->template async_call<Response>(request);
						}
					}
				}

				co_return Response{};
			}

			template <typename Func, typename ConstBufferSequence>
			auto invoke(const std::string& group, uint64_t host_and_port, ConstBufferSequence&& req_buffer,
						const std::string& router, Func&& f) -> asio::awaitable<error_code>
			{
				std::shared_lock lk(mutex_);

				auto iter = pool_.find(group);

				if (iter == pool_.end())
				{
					co_return errc::not_exist_in_pool;
				}

				if (host_and_port == 0)
				{
					auto info_ptr = round_robin<client_info>{}.invoke(iter->second);

					if (info_ptr)
					{
						auto ptr = round_robin<tcp::client>{}.invoke(info_ptr->clients);

						co_return co_await ptr->async_call_buffer(std::forward<ConstBufferSequence>(req_buffer), router,
																  std::forward<Func>(f));
					}
				}
				else
				{
					auto it = std::find_if(iter->second.begin(), iter->second.end(),
										   [&] (auto client_ptr) { return client_ptr->host_port == host_and_port; });

					if (it != iter->second.end())
					{
						auto ptr = round_robin<tcp::client>{}.invoke((*it)->clients);

						co_return co_await ptr->async_call_buffer(std::forward<ConstBufferSequence>(req_buffer), router,
																  std::forward<Func>(f));
					}
				}
			}

			auto shake(const std::string& group, uint64_t host_and_port, const std::string& name, bool healthy,
					   int32_t weight, const std::string& version) -> asio::awaitable<void>;

		private:
			void remove(const std::string& group, uint64_t host_and_port);

			auto add(const std::string& group, uint64_t host_and_port, const std::string& name, bool healthy,
					 int32_t weight, const std::string& version) -> asio::awaitable<void>;

			std::pair<std::string, int32_t> instance_to_host(uint64_t host_and_port);

		private:
			std::shared_mutex mutex_;

			std::map<std::string, std::vector<std::shared_ptr<client_info>>> pool_;

			round_robin<tcp::client> round_;
		};
	} // namespace serviced
} // namespace aquarius