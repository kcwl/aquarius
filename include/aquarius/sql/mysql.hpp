#pragma once
#include <boost/asio.hpp>
#include <aquarius/error_code.hpp>
#include <mysql.h>
#include <aquarius/sql/sql_error.hpp>
#include <aquarius/sql/database_param.hpp>
#include <boost/pfr.hpp>

namespace aquarius
{
	namespace sql
	{
		template <typename Executor>
		class mysql
		{
			struct initiate_connect_task
			{
				initiate_connect_task(mysql* self)
					: self_(self)
				{}

				template <typename Self>
				void operator()(Self& self, error_code ec = {})
				{
					if (self_->mysql_ptr_)
					{
						mysql_close(self_->mysql_ptr_);
					}

					self_->mysql_ptr_ = mysql_init(nullptr);

					if (!self_->mysql_ptr_)
					{
						ec = db_error::bad_alloc;
						self.complete(ec);
						return;
					}

					self_->enable_transaction_ = self_->param_.enable_transaction.value_or(false);

					int32_t timeout = self_->param_.timeout.value_or(-1);

					if (timeout > 0)
					{
						auto err = mysql_options(self_->mysql_ptr_, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
						if (err != 0)
						{
							ec = static_cast<db_error>(mysql_errno(self_->mysql_ptr_));
							self.complete(ec);
							return;
						}
					}

					auto reconnect = self_->param_.reconnect.value_or(true);

					auto err = mysql_options(self_->mysql_ptr_, MYSQL_OPT_RECONNECT, &reconnect);
					if (err != 0)
					{
						ec = static_cast<db_error>(mysql_errno(self_->mysql_ptr_));
						self.complete(ec);
						return;
					}

					if (mysql_real_connect(self_->mysql_ptr_, self_->param_.host.c_str(), self_->param_.user.c_str(),
										   self_->param_.password.c_str(), self_->param_.db.c_str(), self_->param_.port,
										   nullptr, 0) == nullptr)
					{
						ec = static_cast<db_error>(mysql_errno(self_->mysql_ptr_));
						self.complete(ec);
						return;
					}

					ec = db_error::success;
				}

				mysql* self_;
			};

			template <typename T>
			struct initiate_query_task
			{
				initiate_query_task(mysql* self, std::string_view sql)
					: self_(self)
					, sql_(sql)
				{}

				template <typename Self>
				void operator()(Self& self, error_code ec = {}, std::vector<T> results = {})
				{
					auto result = mysql_real_query(self_->mysql_ptr_, sql_.data(), static_cast<uint32_t>(sql_.size()));
					if (result != 0)
					{
						ec = static_cast<db_error>(mysql_errno(self_->mysql_ptr_));
						self.complete(ec, results);
						return;
					}

					result = mysql_field_count(self_->mysql_ptr_);
					if (result == 0)
					{
						ec = db_error::field_empty;
						self.complete(ec, results);
						return;
					}

					auto res = mysql_store_result(self_->mysql_ptr_);

					result = mysql_num_fields(res);

					constexpr size_t current_fields = boost::pfr::tuple_size_v<T>;

					if (current_fields != result)
					{
						ec = db_error::field_number;
						self.complete(ec, results);
						return;
					}

					while (auto row = mysql_fetch_row(res))
					{
						make_result<T>(results, row);
					}

					self.complete(ec, results);
				}

				template <typename T>
				void make_result(std::vector<T>& results, const MYSQL_ROW& row)
				{
					auto to_struct = [&, this]<std::size_t... I>(std::index_sequence<I...>)
					{
						return T{ cast<decltype(boost::pfr::get<I>(std::declval<T>()))>(row[I])... };
					};

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

				mysql* self_;
				std::string_view sql_;
			};

			struct initiate_execute_task
			{
				initiate_execute_task(mysql* self, std::string_view sql)
					: self_(self)
					, sql_(sql)
				{}

				template <typename Self>
				void operator()(Self& self, error_code ec = {}, std::size_t number = 0)
				{
					ec = db_error::success;

					auto result = mysql_real_query(self_->mysql_ptr_, sql_.data(), static_cast<uint32_t>(sql_.size()));
					if (result != 0)
					{
						ec = static_cast<db_error>(mysql_errno(self_->mysql_ptr_));
						self.complete(ec, number);
						return;
					}

					result = mysql_field_count(self_->mysql_ptr_);

					if (result != 0)
					{
						ec = db_error::field_not_empty;
						self.complete(ec, number);
						return;
					}

					self.complete(ec, mysql_affected_rows(self_->mysql_ptr_));
				}

				mysql* self_;
				std::string_view sql_;
			};

		public:
			mysql(const Executor& executor, const database_param& param)
				: executor_(executor)
				, param_(param)
				, mysql_ptr_(nullptr)
				, stmt_ptr_(nullptr)
				, enable_transaction_(true)
			{}

		public:
			template <typename CompleteToken = default_completion_token_t<Executor>>
			auto async_connect(CompleteToken&& token = default_completion_token_t<Executor>()) -> awaitable<void>
			{
				co_await async_compose<CompleteToken, void(error_code)>(initiate_connect_task(this),
																		token, executor_);
			}

			template <typename T, typename CompleteToken = default_completion_token_t<Executor>>
			auto async_query(std::string_view sql, CompleteToken&& token = default_completion_token_t<Executor>()) -> awaitable<std::vector<T>>
			{
				error_code ec{};

				auto result = co_await async_compose<CompleteToken, void(error_code, std::vector<T>)>(
					initiate_query_task<T>(this, sql), token, executor_);

				co_return ec ? std::vector<T>{} : result;
			}

			template <typename CompleteToken = default_completion_token_t<Executor>>
			auto async_execute(std::string_view sql, CompleteToken&& token = default_completion_token_t<Executor>()) -> awaitable<std::size_t>
			{
				co_return co_await async_compose<CompleteToken, void(error_code, std::size_t)>(
					initiate_execute_task(this, sql), token, executor_);
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
			

		private:
			Executor executor_;

			database_param param_;

			MYSQL* mysql_ptr_;
			MYSQL_STMT* stmt_ptr_;

			bool enable_transaction_;
		};
	} // namespace sql
} // namespace aquarius