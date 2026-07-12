#pragma once
#include <aquarius/module/schedule.hpp>
#include <aquarius/tbl/insert_view.hpp>
#include <aquarius/tbl/remove_view.hpp>
#include <aquarius/tbl/select_view.hpp>
#include <aquarius/tbl/update_view.hpp>
#include <database/proto/execute_sql.virgo.h>
#include <serviced/srvd_client.hpp>

namespace aquarius
{
	struct enter_view
	{
		template <typename T>
		auto query(const std::string& sql) -> asio::awaitable<std::vector<T>>
		{
			auto req = std::make_shared<query_sql_request>();
			req->body().sql() = sql;

			auto resp = co_await mpc_async_call<&serviced::srvd_client::async_call<query_sql_response, query_sql_request>>(req);

			std::vector<T> result{};

			make_result(result, resp.body().results());
			
			co_return result;
		}

		auto execute(const std::string& sql) -> asio::awaitable<std::size_t>
		{
			auto req = std::make_shared<execute_sql_request>();
			req->body().sql() = sql;

			auto resp = co_await mpc_async_call<&serviced::srvd_client::async_call<execute_sql_response, execute_sql_request>>(req);

			co_return resp.body().affected();
		}

		template <typename T>
		void make_result(std::vector<T>& results, const std::vector<std::string>& row)
		{
			if (row.empty())
			{
				return;
			}

			auto to_struct = [&, this]<std::size_t... I>(std::index_sequence<I...>)
			{
				return T{ cast<boost::pfr::tuple_element_t<I, T>>(row[I])... };
			};

			constexpr static auto size = boost::pfr::tuple_size_v<T>;

			results.push_back(to_struct(std::make_index_sequence<size>{}));
		}

		template<typename T>
		T cast(const std::string& result)
		{
			std::stringstream ss{};
			ss << result;

			T value{};
			ss >> value;

			return value;
		}
	};

	template <typename T, auto... Args>
	inline auto operator|(const select_view<T, Args...>& v, enter_view& etv) -> asio::awaitable<std::vector<T>>
	{
		co_return co_await etv.query<T>(static_cast<std::string>(v));
	}

	inline auto operator|(const insert_view& v, enter_view& etv) -> asio::awaitable<std::size_t>
	{
		co_return co_await etv.execute(static_cast<std::string>(v));
	}

	inline auto operator|(const update_view& v, enter_view& etv) -> asio::awaitable<std::size_t>
	{
		co_return co_await etv.execute(static_cast<std::string>(v));
	}

	template <typename T>
	inline auto operator|(const remove_view<T>& v, enter_view& etv) -> asio::awaitable<std::size_t>
	{
		co_return co_await etv.execute(static_cast<std::string>(v));
	}

	inline enter_view enter;
} // namespace aquarius