#pragma once
#include <aquarius/tbl/enter_view.hpp>
#include <aquarius/tbl/grep_view.hpp>
#include <aquarius/tbl/insert_view.hpp>
#include <aquarius/tbl/remove_view.hpp>
#include <aquarius/tbl/select_view.hpp>
#include <aquarius/tbl/update_view.hpp>

namespace aquarius
{
	template <typename T, auto Ptr, auto... Args>
	inline select_view<T, Args...>& operator|(select_view<T, Args...>& sel, const grep_view<Ptr>& g)
	{
		sel.merge(g);

		return sel;
	}

	template <auto Ptr1, auto Ptr2>
	inline grep_view<Ptr1>& operator||(grep_view<Ptr1>& g1, const grep_view<Ptr2>& g2)
	{
		g1._or(g2);

		return g1;
	}

	template <typename T, auto... Args>
	inline auto operator|(const select_view<T, Args...>& v, const enter_view& etv) ->asio::awaitable<std::vector<T>>
	{
		co_return co_await etv.select<T>(static_cast<std::string>(v));
	}

	inline auto operator|(const insert_view& v, const enter_view& etv) ->asio::awaitable<std::size_t>
	{
		co_return co_await etv.insert(static_cast<std::string>(v));
	}

	inline auto operator|(const update_view& v, const enter_view& etv)->asio::awaitable<std::size_t>
	{
		co_return co_await etv.update(static_cast<std::string>(v));
	}

	inline auto operator|(const remove_view& v, const enter_view& etv)->asio::awaitable<std::size_t>
	{
		co_return co_await etv.remove(static_cast<std::string>(v));
	}

} // namespace aquarius