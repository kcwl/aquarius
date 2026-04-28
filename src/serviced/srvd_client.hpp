#pragma once
#include <aquarius/module/module_register.hpp>
#include <aquarius/resource/config_tag_invoke.hpp>
#include <aquarius/tcp.hpp>
#include <serviced/proto/regist.virgo.h>

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
		AQUARIUS_MODULE(srvd_client)
		{
			using healty_check_func_t = std::function<void(const std::string&, int32_t, bool)>;

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
				co_return (!co_await client_ptr_->async_connect(host_, port_));
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

			template <typename Func>
			auto subscribe(const std::string& group, Func&& func) -> asio::awaitable<void>
			{
				auto ptr = std::make_shared<subscribe_request>();

				ptr->body().group(group);

				client_ptr_->async_send(ptr, [&](subscribe_resp& resp) { func(resp.body().instances()); });
			}

			template <HEALTHY_CHECK_TOKEN(void(const std::string&, int32_t, bool)) Func>
			void set_healty_check(Func && func)
			{
				healthy_check_func_ = std::forward<Func>(func);
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