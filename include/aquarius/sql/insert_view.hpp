#pragma once
#include <aquarius/detail/concat.hpp>
#include <string>
#include <sstream>
#include <aquarius/detail/struct_name.hpp>
#include <aquarius/sql/add_string.hpp>

namespace aquarius
{
	namespace sql
	{
        template <typename T>
        class insert_view
        {
        public:
            insert_view(T&& value)
            {
                constexpr static auto struct_name = aquarius::detail::struct_name<std::remove_cvref_t<T>>();

                constexpr static auto insert_sql = "insert into "sv;
                constexpr static auto values_sql = " values("sv;

                constexpr static auto complete_sql = concat_v<insert_sql, struct_name, values_sql>;

                complete_sql_ << complete_sql;

                constexpr auto size = boost::pfr::tuple_size_v<std::decay_t<T>>;

                auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
                {
                    ((complete_sql_ << add_string_format(boost::pfr::get<I, T>(value)) << (size != I + 1 ? "," : "")), ...);
                };

                f(std::make_index_sequence<size>{});

                complete_sql_ << ")";
            }

        public:
            auto operator()() -> std::string
            {
                return complete_sql_.str();
            }

        private:
            std::stringstream complete_sql_;
        };

        template<typename T>
        insert_view(T&&) -> insert_view<T>;

        namespace detail
        {
            template<typename T>
            concept can_insert = requires { insert_view(std::declval<T>()); };
        }

        struct _insert
        {
            template <typename T>
                requires detail::can_insert<T>
            constexpr auto operator()(T&& ranges) const
            {
                return insert_view<T>(std::forward<T>(ranges));
            }
        };

        inline constexpr _insert insert;
	}
}