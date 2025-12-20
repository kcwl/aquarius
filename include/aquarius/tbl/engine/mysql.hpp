#pragma once
#include <aquarius/error_code.hpp>
#include <aquarius/tbl/database_param.hpp>
#include <aquarius/tbl/sql_error.hpp>
#include <boost/asio.hpp>
#include <boost/pfr.hpp>
#include <mysql.h>

namespace aquarius
{
	namespace tbl
	{
		class mysql
		{
		public:
			mysql(const database_param& param)
				: param_(param)
				, mysql_ptr_(nullptr)
				, stmt_ptr_(nullptr)
				, enable_transaction_(true)
			{}

		public:
			auto async_connect() -> awaitable<error_code>
			{
				if (mysql_ptr_)
				{
					mysql_close(mysql_ptr_);
				}

				mysql_ptr_ = mysql_init(nullptr);

				if (!mysql_ptr_)
				{
					co_return db_error::bad_alloc;
				}

				enable_transaction_ = param_.enable_transaction.value_or(false);

				int32_t timeout = param_.timeout.value_or(-1);

				if (timeout > 0)
				{
					auto err = mysql_options(mysql_ptr_, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
					if (err != 0)
					{
						co_return static_cast<db_error>(mysql_errno(mysql_ptr_));
					}
				}

				auto reconnect = param_.reconnect.value_or(1);

				mysql_options(mysql_ptr_, MYSQL_OPT_RECONNECT, &reconnect);
				mysql_options(mysql_ptr_, MYSQL_SET_CHARSET_NAME, "utf8");

				if (mysql_real_connect(mysql_ptr_, param_.host.c_str(), param_.user.c_str(), param_.password.c_str(),
									   param_.db.c_str(), param_.port, nullptr, 0) == nullptr)
				{
					co_return static_cast<db_error>(mysql_errno(mysql_ptr_));
				}

				co_return error_code{};
			}

			template <typename T>
			auto async_query(std::string_view sql, error_code& ec) -> awaitable<std::vector<T>>
			{
				auto result = mysql_real_query(mysql_ptr_, sql.data(), static_cast<uint32_t>(sql.size()));
				if (result != 0)
				{
					ec = static_cast<db_error>(mysql_errno(mysql_ptr_));
					co_return std::vector<T>{};
				}

				result = mysql_field_count(mysql_ptr_);
				if (result == 0)
				{
					ec = db_error::field_empty;

					co_return std::vector<T>{};
				}

				auto res = mysql_store_result(mysql_ptr_);

				result = mysql_num_fields(res);

				constexpr size_t current_fields = boost::pfr::tuple_size_v<T>;

				if (current_fields != result)
				{
					ec = db_error::field_number;

					co_return std::vector<T>{};
				}

				std::vector<T> results{};

				while (auto row = mysql_fetch_row(res))
				{
					make_result<T>(results, row);
				}

				co_return results;
			}

			auto async_execute(std::string_view sql, error_code& ec) -> awaitable<std::size_t>
			{
				ec = db_error::success;

				auto result = mysql_real_query(mysql_ptr_, sql.data(), static_cast<uint32_t>(sql.size()));
				if (result != 0)
				{
					ec = static_cast<db_error>(mysql_errno(mysql_ptr_));

					co_return 0;
				}

				result = mysql_field_count(mysql_ptr_);

				if (result != 0)
				{
					ec = db_error::field_not_empty;

					co_return 0;
				}

				co_return mysql_affected_rows(mysql_ptr_);
			}

			bool enable_transaction() const
			{
				return enable_transaction_;
			}

			error_code begin()
			{
				return transation("BEGIN");
			}

			error_code commit()
			{
				return transation("COMMIT");
			}

			error_code rollback()
			{
				return transation("ROLLBACK");
			}

		private:
			error_code transation(const std::string& option)
			{
				auto result = mysql_query(mysql_ptr_, option.c_str());
				if (result == 0)
				{
					return db_error::success;
				}

				return static_cast<db_error>(mysql_errno(mysql_ptr_));
			}

			template <typename T>
			void make_result(std::vector<T>& results, const MYSQL_ROW& row)
			{
				auto to_struct = [&, this]<std::size_t... I>(std::index_sequence<I...>)
				{ return T{ cast<decltype(boost::pfr::get<I>(std::declval<T>()))>(row[I])... }; };

				results.push_back(to_struct(std::make_index_sequence<boost::pfr::tuple_size_v<T>>{}));
			}

			template <typename T>
			auto cast(const char* field)
			{
				std::stringstream ss{};
				ss << field;

				using type = std::remove_cvref_t<T>;

				type result{};

				ss >> result;

				return result;
			}

		private:
			database_param param_;

			MYSQL* mysql_ptr_;

			MYSQL_STMT* stmt_ptr_;

			bool enable_transaction_;
		};
	} // namespace tbl
} // namespace aquarius