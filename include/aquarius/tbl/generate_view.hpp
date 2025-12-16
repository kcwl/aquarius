#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/detail/int_to_string.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/detail/struct_name.hpp>
#include <aquarius/tbl/add_string.hpp>
#include <aquarius/tbl/closure.hpp>
#include <aquarius/tbl/concepts.hpp>
#include <boost/pfr.hpp>

namespace aquarius
{
	namespace tbl
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
					((complete_sql_ << add_string_format(boost::pfr::get<I, T>(value)) << (size != I + 1 ? "," : "")),
					 ...);
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

		template <typename T>
		insert_view(T&&) -> insert_view<T>;

		template <typename T>
		struct remove_view
		{
			constexpr auto operator()() const
			{
				constexpr static auto struct_name = aquarius::detail::struct_name<T>();

				constexpr static auto delete_sql = "delete from "sv;

				return concat_v<delete_sql, struct_name>;
			}
		};

		template <typename T>
		class update_view
		{
		public:
			update_view(T&& value)
			{
				constexpr static auto struct_name = aquarius::detail::struct_name<std::remove_cvref_t<T>>();

				constexpr static auto update_sql = "update "sv;
				constexpr static auto set_sql = " set "sv;

				constexpr auto complete_sql = concat_v<update_sql, struct_name, set_sql>;
				ss_ << complete_sql;

				constexpr auto size = boost::pfr::tuple_size_v<std::decay_t<T>>;

				boost::pfr::for_each_field_with_name(
					value, [&](auto name, auto value, std::size_t index)
					{ ss_ << name.data() << "=" << add_string_format(value) << (index != size - 1 ? " and " : ""); });
			}

		public:
			auto operator()() const -> std::string
			{
				return ss_.str();
			}

		private:
			std::stringstream ss_;
		};

		template <typename T>
		update_view(T&&) -> update_view<T&&>;

		template <typename T>
		struct select_view
		{
			constexpr auto operator()() const -> std::string_view
			{
				constexpr static auto struct_name = aquarius::detail::struct_name<T>();

				constexpr static std::string_view sql = "select * from "sv;

				return concat_v<sql, struct_name>;
			}
		};

		template <detail::string_literal Op>
		struct condition_param
		{
			std::string_view operator()(auto v)
			{
				return std::format("{} {}", Op, v);
			}
		};

		template <typename Cond, typename T>
		struct condition
		{
			constexpr condition(T&& value)
				: value_(std::forward<T>(value))
			{}

			std::string_view operator()() const
			{
				return std::string_view{ Cond()(value_) };
			}

			T value_;
		};

		template <typename T>
		constexpr inline condition<condition_param<"<">, T> less_than;

		template <typename T>
		constexpr inline condition<condition_param<">">, T> greater_than;

		template <typename T>
		constexpr inline condition<condition_param<"<=">, T> less_equal_than;
		template <typename T>
		constexpr inline condition<condition_param<">=">, T> greater_equal_than;

		template <typename T>
		constexpr inline condition<condition_param<"==">, T> equal_than;

		template <typename T>
		constexpr inline condition<condition_param<"!=">, T> not_equal_than;

		template <detail::string_literal Op, typename LCond, typename RCond>
		struct _condition
		{
			constexpr _condition(LCond&& l, RCond&& r)
				: left_(std::forward<LCond>(l))
				, right_(std::forward<RCond>(r))
			{}

			constexpr auto operator()() const -> std::string_view
			{
				return concat_v<left_(), detail::bind_param<Op>::value, right_()>;
			}

			LCond left_;
			RCond right_;
		};

		template <typename LCond, typename RCond>
		constexpr inline _condition<" and ", LCond, RCond> add;

		template <typename LCond, typename RCond>
		constexpr inline _condition<" or ", LCond, RCond> or_;

		template <typename T>
		struct create_view
		{
			constexpr static auto left_space = "("sv;
			constexpr static auto right_space = ")"sv;

			auto operator()() -> std::string
			{
				constexpr static auto sql_str = "CREATE TABLE IF NOT EXISTS "sv;

				auto f = []<std::size_t... I>(std::index_sequence<I...>)
				{ return (boost::pfr::tuple_element_t<I, T>::template get<boost::pfr::get_name<I, T>()>(), ...); };

				constexpr auto size = boost::pfr::tuple_size_v<std::decay_t<T>>;

				constexpr static auto member_str = f(std::make_index_sequence<size>());

				return std::format("create table if not exists {} ({})", aquarius::detail::struct_name<T>(),
								   member_str);
			}
		};
	} // namespace tbl
} // namespace aquarius