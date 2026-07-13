#include "user.h"
#include "enter_view.hpp"
#include "permission_module.h"

namespace aquarius
{
	namespace login
	{
		auto user::check_permission(int64_t perm) -> asio::awaitable<bool>
		{
			for (auto& role_id : role_ids_)
			{
				if (co_await mpc_async_call<&permission_module::check>(role_id, perm))
				{
					co_return true;
				}
			}

			co_return false;
		}

		void user::create_permission(const std::vector<int64_t>& perms)
		{
			if (!role_ids_.empty())
			{
				role_ids_.clear();
			}

			role_ids_ = perms;
		}

		void user::remove_permission(int64_t role_id)
		{
			auto iter = std::find_if(role_ids_.begin(), role_ids_.end(), [&](auto role) { return role == role_id; });

			if (iter == role_ids_.end())
			{
				return;
			}

			role_ids_.erase(iter);
		}

		auto user::view_permission() ->asio::awaitable<std::vector<std::string>>
		{
			std::vector<std::string> result{};

			for (auto& id : role_ids_)
			{
				result.push_back(co_await mpc_async_call<&permission_module::view>(id));
			}

			co_return result;
		}

		std::size_t user::id() const
		{
			return id_;
		}

		auto user::load_role_permission() -> asio::awaitable<void>
		{
			auto res = co_await (select_v<role_permission>() | enter);

			for (auto& r : res)
			{
				fill_role_permission(r);
			}
		}

		void user::fill_role_permission(const role_permission& role_perm)
		{
			role_ids_.push_back(role_perm.role_id);
		}
	} // namespace login
} // namespace aquarius