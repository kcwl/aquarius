#pragma once
#include "auth_module.h"
#include <aquarius/module/schedule.hpp>
#include <memory>
#include <string>

namespace aquarius
{
	namespace gateway
	{
		template <typename R>
		inline auto mpc_auth(std::string_view module_name, const std::string& user, const std::string& passwd) -> awaitable<R>
		{
			co_return co_await mpc::call<R, auth_op>(module_name,
													 [&] (std::shared_ptr<auth_op> op_ptr) ->awaitable<R> { co_return op_ptr->auth(user, passwd); });
		}

	} // namespace gateway
} // namespace aquarius