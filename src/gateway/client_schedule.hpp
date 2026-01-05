#pragma once
#include "client_module.h"

namespace aquarius
{
	namespace gateway
	{
		template <typename R, typename Request>
		inline auto mpc_transfer(std::string_view module_name, std::shared_ptr<Request> request_ptr) -> awaitable<R>
		{
			co_return co_await mpc::call<R, tcp_client>(module_name, [&](std::shared_ptr<tcp_client> op_ptr)
														{ co_return co_await op_ptr->async_call<Request>(request_ptr); });
		}
	} // namespace gateway
} // namespace aquarius