#pragma once
#include <aquarius/module/module_register.hpp>
#include <aquarius/resource/config_tag_invoke.hpp>
#include <aquarius/tcp.hpp>
#include <proto/regist.virgo.h>

#define HEALTHY_CHECK_TOKEN(args) typename

namespace aquarius
{
	struct srv_config
	{
		std::string host;
		int32_t port;
	};

	namespace serviced
	{
		AQUARIUS_MODULE_TOP(srvd_client)
		{
			using healty_check_func_t = std::function<asio::awaitable<void>(const std::string&, int32_t, bool)>;

			using subscribe_func_t = std::function<void(const std::vector<instance>&)>;

		public:
			virtual bool init() override
			{
				srv_config srv;

				cfg_value_from<srv_config>(srv);

				host_ = srv.host;
				port_ = srv.port;

				return true;
			}

			virtual auto run() -> asio::awaitable<bool> override
			{
				client_ptr_ = std::make_shared<tcp_client>(co_await asio::this_coro::executor, 30ms);

				co_return (!co_await client_ptr_->async_connect(host_, static_cast<uint16_t>(port_)));
			}

			auto timer(std::chrono::milliseconds) -> asio::awaitable<void>
			{
				int req_ping = 0;

				co_await client_ptr_->async_send(req_ping);
			}

			auto publish(const std::string& host, int32_t port) -> asio::awaitable<bool>
			{
				auto req = std::make_shared<regist_tcp_request>();

				req->body().host() = host;
				req->body().port() = port;

				flex_buffer buffer{};
				req->commit(buffer);

				co_return !co_await client_ptr_->async_send(std::move(buffer));
			}

			auto subscribe(const std::string& group, const subscribe_func_t& func) -> asio::awaitable<void>
			{
				auto req = std::make_shared<subscribe_service_tcp_request>();

				req->body().group() = group;

				auto resp = co_await client_ptr_->async_call<subscribe_service_tcp_response>(req);

				func(resp.body().instances());
			}

			void set_healty_check(const healty_check_func_t& func)
			{
				healthy_check_func_ = func;
			}

			template <typename... Args>
			void healty_check(Args && ... args)
			{
				if (!healthy_check_func_)
				{
					return;
				}

				healthy_check_func_(std::forward<Args>(args)...);
			}

		private:
			std::shared_ptr<tcp_client> client_ptr_;

			std::string host_;

			int32_t port_;

			healty_check_func_t healthy_check_func_;
		};
	} // namespace serviced
} // namespace aquarius