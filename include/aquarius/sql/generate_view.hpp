#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/sql/closure.hpp>
#include <aquarius/sql/concepts.hpp>

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

		namespace detail
		{
			template <typename T>
			concept can_insert = requires { insert_view(std::declval<T>()); };
		} // namespace detail

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
		struct enable_condition_view<remove_view<T>> : std::true_type
		{};

		template <typename T>
		struct _remove
		{
			constexpr auto operator()() const
			{
				return remove_view<T>();
			}
		};

		template <typename T>
		inline constexpr _remove<T> remove;

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
		struct enable_condition_view<update_view<T>> : std::true_type
		{};

		template <typename T>
		update_view(T&&) -> update_view<T&&>;

		namespace detail
		{
			template <typename T>
			concept can_update = requires { update_view(std::declval<T>()); };
		} // namespace detail

		struct _update
		{
			template <typename T>
			requires detail::can_update<T>
			constexpr auto operator()(T&& ranges) const
			{
				return update_view(std::forward<T>(ranges));
			}
		};

		inline constexpr _update update;

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

		template <typename T>
		struct enable_select_view<select_view<T>> : std::true_type
		{};

		template <typename T>
		struct enable_condition_view<select_view<T>> : std::true_type
		{};

		template <typename T>
		struct _select
		{
			constexpr auto operator()() const
			{
				return select_view<T>();
			}
		};

		template <typename T>
		inline constexpr _select<T> select;

		template <typename T>
		class select_distinct_view
		{
		public:
			constexpr select_distinct_view() = default;

		public:
			constexpr auto operator()() const
			{
				constexpr static auto struct_name = aquarius::detail::struct_name<T>();

				constexpr static std::string_view sql = "select distinct * from "sv;

				return concat_v<sql, struct_name>;
			}

		private:
			std::string sql_;
		};

		template <typename T>
		struct enable_select_view<select_distinct_view<T>> : std::true_type
		{};

		struct _select_distinct : sql_adaptor_closure
		{
			template <typename T>
			constexpr auto operator()() const
			{
				return select_distinct_view<T>();
			}
		};

		inline constexpr _select_distinct distinct;

		template <typename T, typename Cond>
		requires enable_select<T>
		struct where_view
		{
			constexpr auto operator()() const -> std::string_view
			{
				constexpr static auto where_sql = " where "sv;

				constexpr static auto spcae = " "sv;

				constexpr static auto sql = T()();

				using cond_t = std::decay_t<Cond>;

				return concat_v<sql, where_sql, cond_t::left, spcae, cond_t::symbol, spcae, cond_t::right>;
			}
		};

		template <typename T, typename Cond>
		struct enable_where_view<where_view<T, Cond>> : std::true_type
		{};

		namespace detail
		{
			template <typename T, typename... Args>
			concept can_where_adaptor = requires { typename where_view<T, Args...>; };
		} // namespace detail

		struct _where : sql_adaptor<_where>
		{
			template <typename T, typename Cond>
			requires detail::can_where_adaptor<T, Cond>
			constexpr auto operator()(T&&, Cond&&) const
			{
				return where_view<T, Cond>();
			}

			using sql_adaptor<_where>::operator();
		};

		inline constexpr _where where;

		template <typename T, typename Cond>
		requires(enable_condition<T> || enable_where<T>)
		struct and_view
		{
			constexpr auto operator()() const
			{
				constexpr static auto and_sql = " and "sv;

				constexpr static auto sql = T()();

				using cond_t = std::decay_t<Cond>;

				return concat_v<sql, and_sql, cond_t::left, cond_t::symbol, cond_t::right>;
			}
		};

		template <typename T, typename Cond>
		struct enable_condition_view<and_view<T, Cond>> : std::true_type
		{};

		struct _and : sql_adaptor<_and>
		{
			template <typename T, typename Cond>
			constexpr auto operator()(T&&, Cond&&) const
			{
				return and_view<T, Cond>();
			}

			using sql_adaptor<_and>::operator();
		};

		inline constexpr _and and_;

		template <typename Cond, const auto& Table, const auto& Left, const auto& Right>
		struct condition_view
		{
			constexpr static auto table = Table;

			constexpr static auto left = Left;

			constexpr static auto right = Right;

			constexpr static auto symbol = Cond::value;
		};

		struct less
		{
			constexpr static auto value = "<"sv;
		};

		struct greater
		{
			constexpr static auto value = ">"sv;
		};

		struct less_equal
		{
			constexpr static auto value = "<="sv;
		};

		struct greater_equal
		{
			constexpr static auto value = ">="sv;
		};

		struct equal
		{
			constexpr static auto value = "="sv;
		};

		struct not_equal
		{
			constexpr static auto value = "!="sv;
		};

		struct empty
		{
			constexpr static auto value = ""sv;
		};

		template <typename Cond, const auto& Table, const auto& Left, const auto& Right>
		struct _condition : sql_adaptor<_condition<Cond, Table, Left, Right>>
		{
			constexpr auto operator()() const
			{
				return condition_view<Cond, Table, Left, Right>();
			}

			using base = sql_adaptor<_condition<Cond, Table, Left, Right>>;

			using base::operator();
		};

		template <const auto& Table, const auto& Left, const auto& Right>
		inline _condition<less, Table, Left, Right> cond_less;

		template <const auto& Table, const auto& Left, const auto& Right>
		inline _condition<greater, Table, Left, Right> cond_greater;

		template <const auto& Table, const auto& Left, const auto& Right>
		inline _condition<less_equal, Table, Left, Right> cond_less_equal;

		template <const auto& Table, const auto& Left, const auto& Right>
		inline _condition<greater_equal, Table, Left, Right> cond_greater_equal;

		template <const auto& Table, const auto& Left, const auto& Right>
		inline _condition<equal, Table, Left, Right> cond_equal;

		template <const auto& Table, const auto& Left, const auto& Right>
		inline _condition<not_equal, Table, Left, Right> cond_not_equal;

		template <const auto& Cond>
		struct tp_condition_view
		{
			constexpr auto operator()() const -> std::string_view
			{
				return Cond;
			}
		};

		template <const auto& Cond>
		struct _tp_condition : sql_adaptor<_tp_condition<Cond>>
		{
			constexpr auto operator()() const
			{
				return tp_condition_view<Cond>();
			}

			using base = sql_adaptor<_tp_condition<Cond>>;

			using base::operator();
		};

		template <auto Value>
		struct single_number_condition_view
		{
			constexpr auto operator()() const
			{
				return aquarius::detail::int_to_string<Value>();
			}
		};

		template <auto Value>
		struct _single_number : sql_adaptor<_single_number<Value>>
		{
			constexpr auto operator()() const
			{
				return single_number_condition_view<Value>();
			}

			using base = sql_adaptor<_single_number<Value>>;

			using base::operator();
		};

		template <typename T, typename Cond>
		requires enable_select<T>
		struct limit_view
		{
			constexpr auto operator()() const
			{
				constexpr static auto sql = T()();

				constexpr static auto limit_sql = " limit "sv;

				using cond_t = std::decay_t<Cond>;

				constexpr static auto cond_limit = cond_t()()();

				return concat_v<sql, limit_sql, cond_limit>;
			}
		};

		struct _limit : sql_adaptor<_limit>
		{
			template <typename T, typename Cond>
			requires enable_select<T>
			constexpr auto operator()(T&&, Cond&&) const
			{
				return limit_view<T, Cond>();
			}

			using sql_adaptor<_limit>::operator();
		};

		inline constexpr _limit limit;

		template <typename T, typename Cond>
		requires(enable_condition<T> || enable_where<T>)
		struct or_view
		{
			constexpr auto operator()() const
			{
				constexpr static auto and_sql = " or "sv;

				constexpr static auto sql = T()();

				using cond_t = std::decay_t<Cond>;

				return concat_v<sql, and_sql, cond_t::left, cond_t::symbol, cond_t::right>;
			}
		};

		template <typename T, typename Cond>
		struct enable_condition_view<or_view<T, Cond>> : std::true_type
		{};

		struct _or : sql_adaptor<_or>
		{
			template <typename T, typename Cond>
			constexpr auto operator()(T&&, Cond&&) const
			{
				return or_view<T, Cond>();
			}

			using sql_adaptor<_or>::operator();
		};

		inline constexpr _or or_;

		struct order_desc
		{
			constexpr static auto value = " desc"sv;
		};

		struct order_asc
		{
			constexpr static auto value = " asc"sv;
		};

		template <typename T, typename Sort, typename Cond>
		requires enable_select<T>
		struct order_view
		{
			constexpr auto operator()() const
			{
				constexpr static auto sql = T()();

				constexpr static auto order_sql = " order by "sv;

				using cond_t = std::decay_t<Cond>;

				constexpr static auto sort_sql = Sort::value;

				constexpr static auto cond_sql = cond_t()()();

				return concat_v<sql, order_sql, cond_sql, sort_sql>;
			}
		};

		template <typename Sort>
		struct _order : sql_adaptor<_order<Sort>>
		{
			template <typename T, typename Cond>
			requires(enable_select<T>)
			constexpr auto operator()(T&&, Cond&&) const
			{
				return order_view<T, Sort, Cond>();
			}

			using sql_adaptor<_order>::operator();
		};

		inline constexpr _order<order_desc> desc_odr;
		inline constexpr _order<order_asc> asc_odr;
	} // namespace sql
} // namespace aquarius