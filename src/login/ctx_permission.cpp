#include "proto/permission.virgo.h"
#include "user_module.h"
#include <aquarius.hpp>

namespace aquarius
{
	namespace login
	{
		AQUARIUS_HANDLER(query_permission_request, query_permission_response, ctx_permission_query)
		{
			auto& user_id = request()->body().user_id();

			response().body().permissions() = co_await mpc_async_call<&user_module::view_permission>(user_id);

			co_return http_status::ok;
		}

		AQUARIUS_HANDLER(check_permission_request, check_permission_response, ctx_permission_check)
		{
			auto& user_id = request()->body().user_id();
			auto& perm = request()->body().permission();

			auto result = co_await mpc_async_call<&user_module::check_permission>(user_id, perm);

			co_return result ? http_status::ok : http_status::forbidden;
		}

		AQUARIUS_HANDLER(add_permission_request, add_permission_response, ctx_permission_add)
		{
			auto& user_id = request()->body().user_id();
			auto& perms = request()->body().permissions();

			co_await mpc_async_call<&user_module::create_permission>(user_id, perms);

			co_return http_status::ok;
		}
	} // namespace login
} // namespace aquarius