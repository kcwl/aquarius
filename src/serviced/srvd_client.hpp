#pragma once
#include <aquarius/module/module_register.hpp>
#include <serviced/proto/regist.virgo.h>

#define HEALTHY_CHECK_TOKEN(args) typename

struct srv_config : aquarius::singleton<srv_config>
{
	std::string host;
	uint16_t port;
	std::string srvd_host;
	uint16_t srvd_port;
	int32_t weight;
	std::string version;
	std::string local_server_name;
};

namespace aquarius
{
	namespace serviced
	{
		AQUARIUS_MODULE_TOP(srvd_client)
		{
			using healty_check_func_t = std::function<asio::awaitable<void>(uint64_t, bool)>;

		public:
			virtual bool init() override
			{
				host_ = srv_config::get_mutable_instance().srvd_host;
				port_ = srv_config::get_mutable_instance().srvd_port;

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

			template <typename Response, typename Request>
			auto async_call(std::shared_ptr<Request> req) -> asio::awaitable<Response>
			{
				co_return co_await client_ptr_->async_call<Response>(req);
			}

			template <typename Func, typename ConstBufferSequence, typename... Args>
			auto async_call_buffer(ConstBufferSequence && req, const std::string router, Func&& f, Args&&... args)
				-> asio::awaitable<error_code>
			{
				co_return co_await client_ptr_->async_call_buffer(std::forward<ConstBufferSequence>(req), router,
																  std::forward<Func>(f), std::forward<Args>(args)...);
			}

			template <typename Func>
			auto set_healty_check(Func && func) -> asio::awaitable<void>
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