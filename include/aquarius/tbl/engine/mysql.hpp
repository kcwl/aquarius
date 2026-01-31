#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/tbl/database_param.hpp>
#include <aquarius/tbl/reflect.hpp>
#include <aquarius/tbl/sql_error.hpp>
#include <boost/mysql.hpp>
#include <chrono>

using namespace std::chrono_literals;

namespace aquarius
{
	namespace tbl
	{
		class mysql
		{
		public:
			mysql(io_context& io, boost::mysql::pool_params param)
				: mysql(io.get_executor(), std::move(param))
			{}

			template <typename Executor>
			mysql(const Executor& executor, boost::mysql::pool_params param)
				: pool_(executor, std::move(param))
				, enable_transaction_(true)
			{}

		public:
			void async_run()
			{
				pool_.async_run(detached);
			}

			template <typename T>
			auto async_query(std::string_view sql, error_code& ec) -> awaitable<std::vector<T>>
			{
				auto conn_ptr = co_await pool_.async_get_connection(aquarius::cancel_after(1s));

				boost::mysql::results result{};

				co_await conn_ptr->async_execute(sql, result, aquarius::cancel_after(1s, aquarius::redirect_error(use_awaitable, ec)));

				std::vector<T> results{};

				if (!result.has_value())
					co_return results;

				for (const auto r : result.rows())
				{
					make_result<T>(results, r);
				}

				co_return std::move(results);
			}

			auto async_execute(std::string_view sql, error_code& ec) -> awaitable<std::size_t>
			{
				auto conn_ptr = co_await pool_.async_get_connection(aquarius::cancel_after(1s));

				boost::mysql::results result{};

				co_await conn_ptr->async_execute(sql, result);

				co_return result.affected_rows();
			}

			bool enable_transaction() const
			{
				return enable_transaction_;
			}

			auto begin() -> awaitable<error_code>
			{
				return transation("BEGIN");
			}

			auto commit() -> awaitable<error_code>
			{
				return transation("COMMIT");
			}

			auto rollback() -> awaitable<error_code>
			{
				return transation("ROLLBACK");
			}

		private:
			auto transation(const std::string& option) -> awaitable<error_code>
			{
				boost::mysql::results empty_result{};

				error_code ec{};

				boost::mysql::pooled_connection conn_ptr =
					co_await pool_.async_get_connection(aquarius::cancel_after(1s));

				co_await conn_ptr->async_execute(option, empty_result, aquarius::redirect_error(ec));

				co_return ec;
			}

			template <typename T, typename Row>
			void make_result(std::vector<T>& results, const Row& row)
			{
				auto to_struct = [&, this]<std::size_t... I>(std::index_sequence<I...>)
				{ return T{ cast<struct_element_t<I, T>>(row[I])... }; };

				constexpr static auto size = struct_size<T>();

				results.push_back(to_struct(std::make_index_sequence<size>{}));
			}

			template <typename T, typename Field>
			auto cast(const Field& field)
			{
				std::stringstream ss{};
				ss << field;

				std::remove_cvref_t<T> value{};

				value.set_value(ss);

				return value;
			}

		private:
			boost::mysql::connection_pool pool_;

			bool enable_transaction_;
		};
	} // namespace tbl
} // namespace aquarius