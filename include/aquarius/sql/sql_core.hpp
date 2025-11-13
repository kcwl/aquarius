#pragma once
#include <aquarius/sql/database_param.hpp>
#include <aquarius/error_code.hpp>
#include <mysql.h>
#include <aquarius/sql/sql_error.hpp>
#include <aquarius/coroutine.hpp>

namespace aquarius
{
    class sql_core : public std::enable_shared_from_this<sql_core>
    {
        class transaction_guard
        {
        public:
            transaction_guard(std::shared_ptr<sql_core> core_ptr)
                : sql_core_ptr_(core_ptr)
            {
                auto ptr = sql_core_ptr_.lock();
                if (ptr && ptr->enable_transaction())
                {
                    [[maybe_unused]] auto ec = ptr->begin();
                }
            }

            ~transaction_guard()
            {
                auto ptr = sql_core_ptr_.lock();
                if (ptr && ptr->enable_transaction())
                {
                    [[maybe_unused]] auto ec = ptr->commit();
                }
            }

        private:
            std::weak_ptr<sql_core> sql_core_ptr_;
        };

    public:
        sql_core(const database_param& param)
            : param_(param)
        {
        }

        ~sql_core()
        {
            disconnect();
        }

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

            auto reconnect = param_.reconnect.value_or(true);

            auto err = mysql_options(mysql_ptr_, MYSQL_OPT_RECONNECT, &reconnect);
            if (err != 0)
            {
                co_return static_cast<db_error>(mysql_errno(mysql_ptr_));
            }

            if (mysql_real_connect(mysql_ptr_, param_.host.c_str(), param_.user.c_str(),
                                   param_.password.c_str(), param_.db.c_str(), param_.port, nullptr, 0) == nullptr)
            {
                co_return static_cast<db_error>(mysql_errno(mysql_ptr_));
            }

            co_return db_error::success;
        }

        template<typename T>
        auto async_query(std::string_view sql, error_code& ec) -> awaitable<std::vector<T>>
        {
            transaction_guard lk(this->shared_from_this());

            std::vector<T> results{};

            auto result = mysql_real_query(mysql_ptr_, sql.data(), sql.size());
            if (result != 0)
            {
                ec = static_cast<db_error>(mysql_errno(mysql_ptr_));
                co_return results;
            }

            result = mysql_field_count(mysql_ptr_);
            if (result == 0)
            {
                ec = db_error::field_empty;
                co_return results;
            }

            auto res = mysql_store_result(mysql_ptr_);

            result = mysql_num_fields(res);

            constexpr size_t current_fields = boost::pfr::tuple_size_v<T>;

            if (current_fields != result)
            {
                ec = db_error::field_number;
                co_return results;
            }

            while (auto row = mysql_fetch_row(res))
            {
                make_result<T>(results, row);
            }

            co_return results;
        }

        auto async_execute(std::string_view sql, error_code& ec) -> awaitable<int64_t>
        {
            ec = db_error::success;

            transaction_guard lk(this->shared_from_this());

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

    private:
        void disconnect()
        {
            if (!mysql_ptr_)
                return;

            mysql_close(mysql_ptr_);
            mysql_ptr_ = nullptr;
        }

        auto begin() -> awaitable<error_code>
        {
            co_return co_await transation("BEGIN");
        }

        auto commit() -> awaitable<error_code>
        {
            co_return co_await transation("COMMIT");
        }

        auto rollback() -> awaitable<error_code>
        {
            co_return co_await transation("ROLLBACK");
        }

        auto transation(const std::string& option) -> awaitable<error_code>
        {
            auto result = mysql_query(mysql_ptr_, option.c_str());
            if (result == 0)
            {
                co_return db_error::success;
            }

            co_return static_cast<db_error>(mysql_errno(mysql_ptr_));
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

    private:
        MYSQL* mysql_ptr_;
        MYSQL_STMT* stmt_ptr_;

        bool enable_transaction_;

        database_param param_;
    };
}