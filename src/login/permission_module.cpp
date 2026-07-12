#include "permission_module.h"
#include "enter_view.hpp"

namespace aquarius
{
	namespace login
	{
		auto permisson_module::run() -> asio::awaitable<bool>
		{
			co_await load_permission();

			co_return true;
		}

		bool permisson_module::check(int64_t role_id, const std::string& perm)
		{
			auto iter = perms_.find(role_id);
			if (iter == perms_.end())
			{
				return false;
			}

			auto it =
				std::find_if(iter->second.begin(), iter->second.end(), [&](auto& p) { return p->permissions == perm; });

			return it == iter->second.end();
		}

		auto permisson_module::create(int64_t role_id, const std::vector<std::string>& perms) -> asio::awaitable<void>
		{
			std::vector<permisson> result{};
			for (auto& p : perms)
			{
				result.push_back({});
				auto& back = result.back();
				back.role_id = role_id;
				back.permissions = p;
			}

			co_await sql_insert(result);
		}

		auto permisson_module::update(int64_t id, const std::string& perm) -> asio::awaitable<bool>
		{
			auto iter = id_perms_.find(id);
			if (iter == id_perms_.end())
			{
				co_return false;
			}

			auto tmp = iter->second->permissions;
			iter->second->permissions = perm;

			auto affected = co_await sql_update(*iter->second);

			if (affected == 0)
			{
				iter->second->permissions = tmp;
			}
		}

		auto permisson_module::remove(int64_t id) -> asio::awaitable<void>
		{
			auto iter = id_perms_.find(id);

			if (iter == id_perms_.end())
			{
				co_return;
			}

			co_await sql_remove(*iter->second);
		}

		std::vector<std::string> permisson_module::view(int64_t role_id) const
		{
			std::vector<std::string> result;
			auto iter = perms_.find(role_id);
			if (iter != perms_.end())
			{
				for (auto& p : iter->second)
				{
					result.push_back(p->permissions);
				}
			}

			return result;
		}

		auto permisson_module::load_permission() -> asio::awaitable<void>
		{
			auto res = co_await (select_v<permisson>() | enter);

			for (auto& r : res)
			{
				fill_permissions(r);
			}
		}

		void permisson_module::fill_permissions(const permisson& perms)
		{
			auto& p = perms_[perms.role_id];

			p.push_back(std::make_shared<permisson>());

			auto& ptr = p.back();

			ptr->id = perms.id;
			ptr->role_id = perms.role_id;
			ptr->permissions = perms.permissions;

			auto& id_ptr = id_perms_[perms.id];
			if (id_ptr)
			{
				id_ptr.reset();
			}

			id_ptr = ptr;
		}

		auto permisson_module::sql_insert(const std::vector<permisson>& perms) -> asio::awaitable<void>
		{
			auto affected = co_await (insert_v(perms) | enter);

			if (affected != 0)
			{
				for (auto& r : perms)
				{
					fill_permissions(r);
				}
			}
		}

		auto permisson_module::sql_update(const permisson& p) -> asio::awaitable<std::size_t>
		{
			co_return co_await (update_v(p) | enter);
		}

		auto permisson_module::sql_remove(const permisson& p) -> asio::awaitable<void>
		{
			auto affected = co_await (remove_v<permisson>() | grep<&permisson::id> == p.id | enter);

			if (affected != 0)
			{
				perms_.erase(p.role_id);

				id_perms_.erase(p.id);
			}
		}
	} // namespace login
} // namespace aquarius