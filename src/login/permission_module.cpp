#include "permission_module.h"
#include "enter_view.hpp"

namespace aquarius
{
	namespace login
	{
		auto permission_module::run() -> asio::awaitable<bool>
		{
			co_await load_permission();

			co_await load_permission_desc();

			co_return true;
		}

		auto permission_module::add_desc(const std::vector<std::string>& perms) -> asio::awaitable<bool>
		{
			std::vector<perm_desc> pd{};
			for (auto& p : perms)
			{
				pd.push_back({ index_++, p });
			}

			auto affected = co_await (insert_multi_v(pd) | enter);

			if (affected != perms.size())
			{
				co_return false;
			}

			for (auto& p : pd)
			{
				perm_descs_.insert({ p.id, p.perm });
			}

			co_return true;
		}

		auto permission_module::add_role(int64_t id, const std::vector<int64_t>& perms) -> asio::awaitable<bool>
		{
			std::vector<permission> pd{};
			for (auto& p : perms)
			{
				pd.push_back({ perm_index_++, id, p });
			}

			auto affected = co_await (insert_multi_v(pd) | enter);

			if (affected != perms.size())
			{
				co_return false;
			}

			for (auto& p : pd)
			{
				auto& perm = perms_[p.role_id];

				perm.push_back(std::make_shared<permission>(p.id, p.role_id, p.perm_id));
			}

			co_return true;
		}

		auto permission_module::remove(int64_t id) -> asio::awaitable<bool>
		{
			auto iter = perm_descs_.find(id);

			if (iter == perm_descs_.end())
			{
				co_return false;
			}

			auto affected = co_await (remove_v<perm_desc>() | grep<&perm_desc::id> == id | enter);

			if (affected == 0)
			{
				co_return false;
			}

			perm_descs_.erase(iter);
		}

		auto permission_module::remove_role(int64_t role_id) -> asio::awaitable<bool>
		{
			auto iter = perms_.find(role_id);

			if (iter == perms_.end())
			{
				co_return false;
			}

			auto affected = co_await (remove_v<permission>() | grep<&permission::role_id> == role_id | enter);

			if (affected == 0)
			{
				co_return false;
			}

			perms_.erase(iter);
		}

		auto permission_module::update(int64_t id, const std::string& perm) -> asio::awaitable<bool>
		{
			auto iter = perm_descs_.find(id);
			if (iter == perm_descs_.end())
			{
				co_return false;
			}

			auto affected = co_await (update_v(perm_desc{ iter->first, iter->second }) | enter);

			if (affected == 0)
			{
				co_return false;
			}

			iter->second = perm;
		}

		auto permission_module::view(int64_t id) const -> asio::awaitable<std::string>
		{
			auto iter = perm_descs_.find(id);
			if (iter == perm_descs_.end())
			{
				co_return std::string{};
			}

			co_return iter->second;
		}

		auto permission_module::check(int64_t role_id, int64_t perm) -> asio::awaitable<bool>
		{
			auto iter = perms_.find(role_id);

			if (iter == perms_.end())
			{
				co_return false;
			}

			auto it = std::find_if(iter->second.begin(), iter->second.end(), [&] (auto p) { return p->perm_id == perm; });

			co_return it != iter->second.end();
		}

		auto permission_module::load_permission() -> asio::awaitable<void>
		{
			auto res = co_await (select_v<permission>() | enter);

			for (auto& r : res)
			{
				fill_permissions(r);
			}
		}

		auto permission_module::load_permission_desc() -> asio::awaitable<void>
		{
			auto res = co_await (select_v<perm_desc> | enter);

			for (auto& r : res)
			{
				fill_permissions_desc(r);
			}
		}

		void permission_module::fill_permissions(const permission& perms)
		{
			auto& p = perms_[perms.role_id];

			p.push_back(std::make_shared<permission>());

			auto& ptr = p.back();

			*ptr = perms;
		}

		void permission_module::fill_permissions_desc(const perm_desc& perms)
		{
			auto& perm = perm_descs_[perms.id];

			perm = perms.perm;
		}
	} // namespace login
} // namespace aquarius