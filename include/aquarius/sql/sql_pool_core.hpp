#pragma once
#include <aquarius/sql/sql_core.hpp>
#include <aquarius/detail/tag.hpp>
#include <aquarius/tag_invoke.hpp>
#include <aquarius/coroutine.hpp>
#include <aquarius/detail/io_service_pool.hpp>

namespace aquarius
{

    class sql_pool_core
    {
    public:
        sql_pool_core() = default;
        virtual ~sql_pool_core() = default;

    public:
        auto run(const database_param& param) -> awaitable<void>
        {
            for (int i = 0; i < param.pool_size; ++i)
            {

                auto sql_ptr = std::make_shared<sql_core>(param);

                co_await sql_ptr->async_connect();

                sql_pool_.push_back(sql_ptr);
            }
        }

        std::shared_ptr<sql_core> get_sql_adaptor()
        {
            if (next_sql_adaptor_ == sql_pool_.size())
                next_sql_adaptor_ = 0;

            return sql_pool_[next_sql_adaptor_++];
        }

    private:
        std::vector<std::shared_ptr<sql_core>> sql_pool_;

        std::size_t next_sql_adaptor_;
    };

    inline sql_pool_core& sql_pool()
    {
        static sql_pool_core pool;

        static std::once_flag once;

        std::call_once(once, [&] ->awaitable<void>
                       {
                           database_param param{};

                           tag_invoke(value_to<db_tag>{}, param);

                           if (param.pool_size != 0)
                           {
                               co_await co_spawn(attach_io_context(), pool.run(param), use_awaitable);
                           }
                       });

        return pool;
    }
}