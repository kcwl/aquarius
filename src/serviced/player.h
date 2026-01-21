#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/ip/http/http_client.hpp>
#include <aquarius/ip/tcp/tcp_client.hpp>
#include <aquarius/module/player_module.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <atomic>
#include <memory>
#include <string>

namespace aquarius
{
	namespace serviced
	{
		class player : public player_base
		{
		public:
			player(std::size_t id);

			virtual ~player() = default;

		public:
			std::size_t id() const;

			auto feedback(flex_buffer& buffer, error_code& ec, std::size_t id) -> awaitable<flex_buffer>;

			void complete();

			int32_t weight() const;

			void weight(int32_t w);

			void set_addr(const std::string& ip_addr, int32_t port);

			void set_protocol(int32_t proto);

			std::string ip_addr() const;

			int32_t port() const;

			int32_t protocol() const;

			template <typename Executor>
			auto create_client(const Executor& ex) -> awaitable<bool>
			{
				if (protocol_ == 0)
				{
					tcp_client_ptr_ = std::make_shared<tcp_client>(ex);

					co_return co_await tcp_client_ptr_->async_connect(ip_addr_, std::to_string(port_));
				}
				else if (protocol_ == 1)
				{
					http_client_ptr_ = std::make_shared<http_client>(ex);

					co_return co_await http_client_ptr_->async_connect(ip_addr_, std::to_string(port_));
				}
			}

		private:
			std::size_t id_;

			std::atomic_size_t task_total_;

			int32_t weight_;

			std::string ip_addr_;

			int32_t port_;

			int32_t protocol_;

			std::shared_ptr<aquarius::tcp_client> tcp_client_ptr_;

			std::shared_ptr<aquarius::http_client> http_client_ptr_;
		};
	} // namespace serviced
} // namespace aquarius