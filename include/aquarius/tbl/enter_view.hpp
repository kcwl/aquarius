#pragma once
#include <aquarius/module/mysql_module.hpp>
#include <aquarius/module/schedule.hpp>
#include <aquarius/tbl/insert_view.hpp>
#include <aquarius/tbl/remove_view.hpp>
#include <aquarius/tbl/select_view.hpp>
#include <aquarius/tbl/update_view.hpp>

namespace aquarius
{
	struct enter_view
	{
		template <typename T>
		auto select(const std::string& sql) const -> asio::awaitable<std::vector<T>>
		{
			co_return co_await mpc_async_call<&mysql_module::async_query<T>>(std::string_view(sql));
		}

		auto insert(const std::string& sql) const -> asio::awaitable<std::size_t>
		{
			co_return co_await mpc_async_call<&mysql_module::async_execute>(std::string_view(sql));
		}

		auto remove(const std::string& sql) const -> asio::awaitable<std::size_t>
		{
			co_return co_await mpc_async_call<&mysql_module::async_execute>(std::string_view(sql));
		}

		auto update(const std::string& sql) const -> asio::awaitable<std::size_t>
		{
			co_return co_await mpc_async_call<&mysql_module::async_execute>(std::string_view(sql));
		}
	};

	template <typename T, auto... Args>
	inline auto operator|(const select_view<T, Args...>& v, const enter_view& etv) -> asio::awaitable<std::vector<T>>
	{
		co_return co_await etv.select<T>(static_cast<std::string>(v));
	}

	inline auto operator|(const insert_view& v, const enter_view& etv) -> asio::awaitable<std::size_t>
	{
		co_return co_await etv.insert(static_cast<std::string>(v));
	}

	inline auto operator|(const update_view& v, const enter_view& etv) -> asio::awaitable<std::size_t>
	{
		co_return co_await etv.update(static_cast<std::string>(v));
	}

	template <typename T>
	inline auto operator|(const remove_view<T>& v, const enter_view& etv) -> asio::awaitable<std::size_t>
	{
		co_return co_await etv.remove(static_cast<std::string>(v));
	}

	inline enter_view enter;
} // namespace aquarius