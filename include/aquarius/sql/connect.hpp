#pragma once
#include <aquarius/sql/empty.hpp>
#include <aquarius/sql/transcation.hpp>

#if defined MYSQL_SQL
#include <aquarius/sql/mysql.hpp>
#endif

namespace aquarius
{

    template<typename Executor>
    class sql_connect
    {
#if defined (MYSQL_SQL)
        using service_impl = mysql;
#else
        using service_impl = empty_impl;
#endif

    public:
        sql_connect(const Executor& exector, const database_param& param)
            : executor_(exector)
            , service_()
            , param_(param)
            , transaction_(service_)
        {

        }

        template<typename Context>
        sql_connect(Context& context, const database_param& param)
            : sql_connect(context.get_executor(), param)
        {

        }

    public:
        Executor get_executor() const
        {
            return executor_;
        }

        template<typename CompletionToken>
        auto async_connect(CompletionToken&& token)
        {
            return service_.async_connect(param_, token, get_executor());
        }

        auto async_connect() -> boost::asio::awaitable<error_code>
        {
            error_code ec;
            co_await async_connect(boost::asio::redirect_error(boost::asio::detached, ec));

            co_return ec;
        }

        template<typename T, typename... Args>
        auto async_select(Args&&... args)
        {
            std::string_view sql{};

            return service_.async_query(sql, get_executor());
        }

        template<typename T>
        auto async_insert(T&& t)
        {
            trans_guard lk(*this);

            std::string_view sql{};

            return service_.async_execute(sql, get_executor());
        }

        template<typename T, typename... Args>
        auto async_update(T&& t, Args&&... args)
        {
            trans_guard lk(*this);

            std::string_view sql{};

            return service_.async_execute(sql, get_executor());
        }

        template<typename T, typename... Args>
        auto async_delete(T&& t, Args&&... args)
        {
            trans_guard lk(*this);

            std::string_view sql{};

            return service_.async_execute(sql, get_executor());
        }

        void enable_transaction()
        {
            transaction_.enable();
        }

        void disable_transaction()
        {
            transaction_.disable();
        }

    private:
        Executor executor_;

        service_impl service_;

        database_param param_;

        transcation<service_impl> transaction_;
    };
}