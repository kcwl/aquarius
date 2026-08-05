#include "srvd_bridge.h"
#include "basic_transfer_context.hpp"
#include "gate_error_code.h"
#include "proto/shake.virgo.h"
#include <serviced/srvd_client.hpp>

namespace aquarius
{
	namespace gateway
	{
		srvd_bridge::srvd_bridge()
			: group_("gateway")
			, srv_host_()
			, srv_port_()
		{}

		bool srvd_bridge::init()
		{
			srv_host_ = srv_config::get_mutable_instance().host;
			srv_port_ = srv_config::get_mutable_instance().port;

			return true;
		}

		auto srvd_bridge::run() -> asio::awaitable<bool>
		{
			co_await mpc_async_call<&serviced::srvd_client::publish>(group_, srv_host_, srv_port_);

			auto request = std::make_shared<shake_request>();

			auto resp =
				co_await mpc_async_call<&serviced::srvd_client::async_call<shake_response, shake_request>>(request);

			co_return co_await set_topics(resp.body().topics());
		}

		auto srvd_bridge::set_topics(const std::vector<std::string>& topics) -> asio::awaitable<bool>
		{
			auto ctx_func = [this]<typename Func>(flex_buffer& buffer, const std::string& router,
												  Func&& f) -> asio::awaitable<error_code>
			{
				auto tf = [func = std::move(f)](flex_buffer& buf, const std::string&) -> asio::awaitable<error_code>
				{ co_return co_await func(buf, {}); };

				co_return co_await mpc_async_call<
					&serviced::srvd_client::async_call_buffer<decltype(tf), flex_buffer&>>(std::ref(buffer), router,
																						   tf);
			};

			std::shared_ptr<context_base> ctx = std::make_shared<basic_transfer_context<tcp>>(ctx_func);

			for (auto& topic : topics)
			{
				XLOG_INFO() << "Regist Context: " << topic;
				mpc_put_context(topic, ctx, true);
			}

			co_return true;
		}

		std::pair<std::string, int32_t> srvd_bridge::instance_to_host(uint64_t host_and_port)
		{
			uint32_t host = host_and_port >> 32;
			int32_t port = static_cast<int32_t>(host_and_port);

			return { asio::ip::address_v4(host).to_string(), port };
		}
	} // namespace gateway
} // namespace aquarius