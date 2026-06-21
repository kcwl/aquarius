#include "srv_regist.h"
#include <serviced/srvd_client.hpp>

namespace aquarius
{
	namespace login
	{
		srv_regist::srv_regist()
			: group_("services")
		{}

		bool srv_regist::init()
		{
			host_ = "127.0.0.1";
			port_ = 3344;

			return true;
		}

		auto srv_regist::run() -> asio::awaitable<bool>
		{
			co_await mpc_async_call<&serviced::srvd_client::publish>(group_, host_, port_);

			co_return true;
		}
	} // namespace login
} // namespace aquarius