#pragma once
#include <aquarius/sql/concepts.hpp>
#include <aquarius/detail/concat.hpp>
#include <aquarius/sql/closure.hpp>
#include <aquarius/detail/int_to_string.hpp>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace sql
	{

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


		template<auto Value>
		struct single_number_condition_view
		{
			constexpr auto operator()() const
			{
				return aquarius::detail::int_to_string<Value>();
			}
		};

		template<auto Value>
		struct _single_number : sql_adaptor<_single_number<Value>>
		{
			constexpr auto operator()() const
			{
				return single_number_condition_view<Value>();
			}

			using base = sql_adaptor<_single_number<Value>>;

			using base::operator();
		};
	} // namespace sql
} // namespace aquarius