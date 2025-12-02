#pragma once
#include <aquarius/sql/database_param.hpp>

namespace aquarius
{
	namespace sql
	{

        class empty_impl
        {
        public:
            empty_impl() = default;
            virtual ~empty_impl() = default;

        public:
            template<typename Executor>
            void async_connect(const database_param& param, const Executor& executor)
            {
                return;
            }

            template<typename Executor>
            std::size_t async_execute(std::string_view sql, const Executor& executor)
            {
                return 0;
            }

            template<typename Executor>
            auto async_query(std::string_view sql, const Executor& executor)
            {
                return {};
            }

            void enable_transaction()
            {
                return;
            }

            void disable_transaction()
            {
                return;
            }
        };

	}
}
