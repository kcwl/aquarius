#include "srv_regist.h"
#include <serviced/proto/shake.virgo.h>
#include <serviced/srvd_client.hpp>

namespace aquarius
{
	namespace login
	{
		srv_regist::srv_regist()
			: group_("database")
			, host_()
			, port_()
		{}

		bool srv_regist::init()
		{
			host_ = "127.0.0.1";
			port_ = srv_config::get_mutable_instance().port;

			return true;
		}

		auto srv_regist::run() -> asio::awaitable<bool>
		{
			auto request = std::make_shared<shake_request>();
			request->body().host() = host_;
			request->body().port() = port_;
			request->body().version() = srv_config::get_mutable_instance().version;
			request->body().weight() = srv_config::get_mutable_instance().weight;
			request->body().group() = group_;
			mpc_generate_topics(request->body().topics());

			co_await mpc_async_call<&serviced::srvd_client::async_call<shake_response, shake_request>>(request);

			co_return true;
		}
	} // namespace login
} // namespace aquarius