#pragma once
#include "customer_base.h"

namespace aquarius
{
	namespace serviced
	{
		template <typename T>
		class basic_customer : public customer_base
		{
		public:
			using client_t = T;

			using func_t = std::function<asio::awaitable<std::expected<flex_buffer, error_code>>(flex_buffer&, int32_t)>;

		public:
			basic_customer()
				: client_ptr_(nullptr)
			{}
			virtual ~basic_customer() = default;

		public:
			void set_addr(const std::string& ip_addr, int32_t port)
			{
				ip_addr_ = ip_addr;
				port_ = port;
			}

			std::string ip_addr() const
			{
				return ip_addr_;
			}

			int32_t port() const
			{
				return port_;
			}

			template <typename Executor>
			auto create_client(const Executor& ex) -> asio::awaitable<bool>
			{
				client_ptr_ = std::make_shared<client_t>(ex);

				co_return co_await client_ptr_->async_connect(ip_addr_, static_cast<uint16_t>(port_));
			}

			auto async_send(flex_buffer& buffer, int method) -> asio::awaitable<std::expected<flex_buffer, error_code>>
			{
				if (!client_ptr_)
				{
					co_return std::unexpected(error_code{});
				}

				co_return co_await client_ptr_->async_send(std::move(buffer));
			}

			template<typename Response, typename Request>
			auto async_call(std::shared_ptr<Request> req) ->asio::awaitable<Response>
			{
				co_return co_await client_ptr_->template async_send<Response>(req);
			}

		private:
			std::shared_ptr<client_t> client_ptr_;

			std::string ip_addr_;

			int32_t port_;
		};
	} // namespace serviced
} // namespace aquarius