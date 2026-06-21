#pragma once
#include <aquarius/module/module_register.hpp>
#include <proto/regist.virgo.h>

#define HEALTHY_CHECK_TOKEN(args) typename

struct srv_config : aquarius::singleton<srv_config>
{
	std::string host;
	int32_t port;
};

namespace aquarius
{
	namespace serviced
	{
		AQUARIUS_MODULE_TOP(srvd_client)
		{
			using healty_check_func_t = std::function<asio::awaitable<void>(uint64_t, bool)>;

			using subscribe_func_t = std::function<asio::awaitable<void>(const std::vector<uint64_t>&)>;

		public:
			virtual bool init() override
			{
				host_ = srv_config::get_mutable_instance().host;
				port_ = srv_config::get_mutable_instance().port;

				return true;
			}

			virtual auto run() -> asio::awaitable<bool> override
			{
				client_ptr_ = std::make_shared<tcp::client>(this->executor_, 30ms);

				co_return (!co_await client_ptr_->async_connect(host_, static_cast<uint16_t>(port_)));
			}

			auto timer(std::chrono::milliseconds) -> asio::awaitable<void>
			{
				co_return;
			}

			auto publish(const std::string& group, const std::string& host, int32_t port) -> asio::awaitable<bool>
			{
				auto req = std::make_shared<regist_request>();

				req->body().host() = host;
				req->body().port() = port;
				req->body().group() = group;

				auto resp = co_await client_ptr_->async_call<regist_response>(req);
				if (resp.result() != 0)
				{
					XLOG_ERROR() << "regist serviced failed! error:" << resp.result();
				}

				co_return !resp.result();
			}

			auto subscribe(const std::string& group, subscribe_func_t func) -> asio::awaitable<void>
			{
				auto f = std::move(func);
				auto req = std::make_shared<subscribe_service_request>();

				req->body().group() = group;

				auto resp = co_await client_ptr_->async_call<subscribe_service_response>(req);

				co_await f(resp.body().instances());
			}

			template <typename Func>
			auto set_healty_check(Func && func) ->asio::awaitable<void>
			{
				healthy_check_func_ = func;
				co_return;
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
			std::shared_ptr<tcp::client> client_ptr_;

			std::string host_;

			int32_t port_;

			healty_check_func_t healthy_check_func_;
		};
	} // namespace serviced
} // namespace aquarius