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
				std::string host;
				uint16_t port;
				std::string name;
				bool healthy;
				int32_t weight;
				std::string version;
				std::vector<std::string> topics;
				std::vector<std::shared_ptr<tcp::client>> clients;
			};

		public:
			template <typename Response, typename Request>
			auto invoke(const std::string& group, const std::string& host, uint16_t port,
						std::shared_ptr<Request> request) -> asio::awaitable<Response>
			{
				auto ptr = get_client(group, host, port);

				if (!ptr)
				{
					XLOG_ERROR() << "server [" << group << "] {" << host << ":" << port << "} is not exsited";
					co_return Response();
				}

				co_return co_await ptr->template async_call<Response>(request);
			}

			template <typename Func, typename ConstBufferSequence>
			auto invoke_buffer(const std::string& group, const std::string& host, uint16_t port, ConstBufferSequence&& buffers,
						const std::string& router, Func&& f) -> asio::awaitable<error_code>
			{
				auto ptr = get_client(group, host, port);
				if (!ptr)
				{
					XLOG_ERROR() << "server [" << group << "] {" << host << ":" << port << "} is not exsited";
					co_return errc::not_exist_in_pool;
				}

				co_return co_await ptr->async_call_buffer(std::forward<ConstBufferSequence>(buffers), router,
														  std::forward<Func>(f));
			}

			auto add(const std::string& group, const std::string& host, uint16_t port, const std::string& name,
					 int32_t weight, const std::string& version, const std::vector<std::string>& topics)
				-> asio::awaitable<bool>;

			auto generate_topics(std::vector<std::string> & topics)->asio::awaitable<bool>;

		private:
			void remove(const std::string& group, const std::string& host, uint16_t port);

			std::shared_ptr<tcp::client> get_client(const std::string& group, const std::string& host, uint16_t port);

		private:
			std::shared_mutex mutex_;

			std::map<std::string, std::vector<std::shared_ptr<client_info>>> pool_;

			round_robin<tcp::client> round_;
		};
	} // namespace serviced
} // namespace aquarius