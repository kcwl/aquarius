#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/detail/int_to_string.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/detail/struct_name.hpp>
#include <aquarius/tbl/add_string.hpp>
#include <aquarius/tbl/concepts.hpp>
#include <aquarius/tbl/engine/common.hpp>
#include <aquarius/tbl/reflect.hpp>

namespace aquarius
{
	namespace tbl
	{
		struct insert_view
		{
			template <typename T>
			auto operator()(T&& value) -> std::string
			{
				complete_sql.str("");

				constexpr auto struct_name = aquarius::detail::struct_name<std::remove_cvref_t<T>>();

				complete_sql << "insert into " << struct_name << "(";

				using type = std::remove_cvref_t<T>;

				constexpr auto size = struct_size<T>();

				auto type_f = [&]<std::size_t... I>(std::index_sequence<I...>)
				{
					((!has_attribute<AI, struct_element_t<I, T>>()
						  ? (complete_sql << struct_element_name<I, T>() << (size != I + 1 ? "," : ""))
						  : complete_sql),
					 ...);
				};

				type_f(std::make_index_sequence<size>{});

				complete_sql << ") values (";

				auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
				{
					((!has_attribute<AI, struct_element_t<I, T>>()
						  ? complete_sql << struct_element_value<I, T>(value) << (size != I + 1 ? "," : "")
						  : complete_sql),
					 ...);
				};

				f(std::make_index_sequence<size>{});

				complete_sql << ")";

				return complete_sql.str();
			}

			std::stringstream complete_sql;
		};

		struct remove_view
		{
			template <typename T>
			auto operator()(T&& value) -> std::string
			{
				constexpr auto struct_name = aquarius::detail::struct_name<std::decay_t<T>>();

				complete_sql.str("");

				complete_sql << "delete from " << struct_name;

				int start = 0;

				auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
				{
					(((has_attribute<PK, struct_element_t<I, T>>() && !has_attribute<AI, struct_element_t<I, T>>())
						  ? (complete_sql << (start++ == 0 ? " where " : " and ") << struct_element_name<I, T>() << "="
										  << struct_element_value<I, T>(value))
						  : complete_sql),
					 ...);
				};

				f(std::make_index_sequence<struct_size<T>()>{});

				return complete_sql.str();
			}

			std::stringstream complete_sql;
		};

		struct update_view
		{
			template <typename T>
			auto operator()(T&& value) -> std::string
			{
				complete_sql.str("");

				constexpr auto struct_name = aquarius::detail::struct_name<std::remove_cvref_t<T>>();

				complete_sql << "update " << struct_name;

				constexpr auto size = struct_size<T>();

				int start = 0;

				auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
				{
					(((has_attribute<PK, struct_element_t<I, T>>() && !has_attribute<AI, struct_element_t<I, T>>())
						  ? (complete_sql << (start++ == 0 ? " set " : " and ") << struct_element_name<I, T>() << "="
										  << struct_element_value<I, T>(value))
						  : complete_sql),
					 ...);
				};

				f(std::make_index_sequence<size>{});

				return complete_sql.str();
			}

			std::stringstream complete_sql;
		};

		struct select_view
		{
			template <typename T>
			auto operator()(T&& value) -> std::string
			{
				complete_sql.str("");

				constexpr auto struct_name = aquarius::detail::struct_name<std::remove_cvref_t<T>>();

				complete_sql << "select * from " << struct_name;

				int start = 0;

				auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
				{
					(((has_attribute<PK, struct_element_t<I, T>>() && !has_attribute<AI, struct_element_t<I, T>>())
						  ? (complete_sql << (start++ == 0 ? " where " : " and ") << struct_element_name<I, T>() << "="
										  << struct_element_value<I, T>(value))
						  : complete_sql),
					 ...);
				};

				f(std::make_index_sequence<struct_size<T>()>{});

				return complete_sql.str();
			}

			std::stringstream complete_sql;
		};

		template <typename T>
		struct create_view
		{
			auto operator()() -> std::string
			{
				constexpr auto struct_name = aquarius::detail::struct_name<std::remove_cvref_t<T>>();

				complete_sql.str("");

				complete_sql << "create table if not exists " << struct_name << "(";

				constexpr auto size = struct_size<T>();

				auto for_each_attribute = [&]<typename R, std::size_t... I>(std::index_sequence<I...>)
				{
					constexpr auto r_size = std::tuple_size_v<R>;
					((complete_sql << std::tuple_element_t<I, R>::value << I), ...);
				};

				auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
				{
					((complete_sql << struct_element_name<I, T>() << " "
								   << remove_class_pointer<std::decay_t<struct_element_t<I, T>>>::type::get_type_name()
								   << collect_attribute<struct_element_t<I, T>>()),
					 ...);
				};

				f(std::make_index_sequence<size>{});

				return complete_sql.str();
			}

			std::stringstream complete_sql;
		};

		static insert_view insert;
		static remove_view remove;
		static update_view update;
		static select_view select;

		template <typename T>
		static create_view<T> create;
	} // namespace tbl
} // namespace aquarius