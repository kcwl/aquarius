#include "user_module.h"
#include "enter_view.hpp"

namespace aquarius
{
	namespace login
	{
		auto user_module::insert(std::shared_ptr<user> user_ptr) -> asio::awaitable<void>
		{
			users_.emplace(user_ptr->id(), user_ptr);

			co_return;
		}

		auto user_module::erase(std::size_t id) -> asio::awaitable<void>
		{
			users_.erase(id);
			co_return;
		}

		auto user_module::check_permission(std::size_t user_id, int64_t perm) -> asio::awaitable<bool>
		{
			auto it = users_.find(user_id);

			if (it == users_.end())
			{
				co_return false;
			}

			co_return co_await it->second->check_permission(perm);
		}

		auto user_module::create_permission(std::size_t user_id, const std::vector<int64_t>& perms)
			-> asio::awaitable<void>
		{
			auto iter = users_.find(user_id);

			if (iter == users_.end())
			{
				co_return;
			}

			iter->second->create_permission(perms);
		}

		auto user_module::remove_permission(std::size_t user_id, int64_t id) -> asio::awaitable<void>
		{
			auto iter = users_.find(user_id);

			if (iter == users_.end())
			{
				co_return;
			}

			iter->second->remove_permission(id);
		}

		auto user_module::view_permission(std::size_t user_id) -> asio::awaitable<std::vector<std::string>>
		{
			auto iter = users_.find(user_id);

			if (iter == users_.end())
			{
				co_return false;
			}

			co_return co_await iter->second->view_permission();
		}

		auto user_module::load_role_permission() -> asio::awaitable<void>
		{
			for (auto& user : users_)
			{
				co_await user.second->load_role_permission();
			}
		}
	} // namespace login
} // namespace aquarius