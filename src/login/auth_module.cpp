#include "auth_module.h"
#include "enter_view.hpp"
#include "error.hpp"

namespace aquarius
{
	namespace login
	{
		auto auth_module::run() -> asio::awaitable<bool>
		{
			co_await load_account();

			co_return true;
		}

		auto auth_module::create_account(const std::string& username, const std::string& password)
			-> asio::awaitable<error_code>
		{
			account acc{ 0, username, password };

			co_return co_await sql_insert(acc);
		}

		auto auth_module::remove_account(const std::string& username) -> asio::awaitable<error_code>
		{
			account acc{ 0, username };

			co_return co_await sql_remove(acc);
		}

		auto auth_module::change_password(const std::string& username, const std::string& old_passwd,
										  const std::string& new_passwd, const std::string& new_passwd_sec)
			-> asio::awaitable<error_code>
		{
			if (new_passwd != new_passwd_sec)
			{
				co_return error::not_exist_account;
			}

			auto iter = accounts_.find(username);
			if (iter == accounts_.end())
			{
				co_return error::not_exist_account;
			}

			if (iter->second->password != old_passwd)
			{
				co_return error::incorrect_passwd;
			}

			iter->second->password = new_passwd;

			auto affected = co_await sql_update(*iter->second);

			if (affected == 0)
			{
				iter->second->password = old_passwd;
			}
		}

		auto auth_module::check_password(const std::string& username, const std::string& password)
			-> asio::awaitable<error_code>
		{
			auto iter = accounts_.find((username));
			if (iter == accounts_.end())
			{
				co_return error::not_exist_account;
			}

			co_return iter->second->password == password ? error::success : error::incorrect_passwd;
		}

		auto auth_module::load_account() -> asio::awaitable<void>
		{
			auto res = co_await (select_v<account>() | enter);

			for (auto& r : res)
			{
				fill_account(r);
			}
		}

		void auth_module::fill_account(const account& acc)
		{
			auto& a = accounts_[acc.acc];
			if (!a)
			{
				a = std::make_shared<account>();
			}

			a->acc = acc.acc;
			a->password = acc.password;
		}

		auto auth_module::sql_insert(const account& acc) -> asio::awaitable<error_code>
		{
			auto affected = co_await (insert_v(acc) | enter);
			if (affected != 0)
			{
				fill_account(acc);
			}

			co_return error_code{};
		}

		auto auth_module::sql_remove(const account& acc) -> asio::awaitable<error_code>
		{
			auto affected = co_await (insert_v(acc) | enter);
			if (affected != 0)
			{
				accounts_.erase(acc.acc);
			}

			co_return error_code{};
		}

		auto auth_module::sql_update(const account& acc) -> asio::awaitable<std::size_t>
		{
			co_return co_await (update_v(acc) | enter);
		}
	} // namespace login
} // namespace aquarius