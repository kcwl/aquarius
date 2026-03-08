#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/detail/int_to_string.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/detail/struct_name.hpp>
#include <aquarius/tbl/add_string.hpp>
#include <aquarius/tbl/reflect.hpp>

namespace aquarius
{
	namespace tbl
	{
		namespace detail
		{
			template <size_t I, typename T>
			bool has_condition(T&& value)
			{
				auto result = struct_element_value<I, T>(value);

				return result.has_condition();
			}

			template <size_t I, typename T>
			bool has_value(const T& value)
			{
				return struct_element_value<I, T>(value).has_value();
			}

			template <typename T>
			void make_sql_condition(std::stringstream& ss, T&& value, const std::string& key = " where ")
			{
				int start = 0;
				auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
				{
					((has_condition<I, T>(value)
						  ? (ss << (start++ == 0 ? key : " and ") << struct_element_name<I, T>() << "="
								<< struct_element_value<I, T>(value))
						  : ss),
					 ...);
				};

				f(std::make_index_sequence<struct_size<T>()>());
			}

			template <typename T>
			void make_value_update(std::stringstream& ss, T&& value)
			{
				int start = 0;
				auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
				{
					((has_value<I, T>(value) ? (ss << (start++ == 0 ? " set " : " and ") << struct_element_name<I, T>()
												   << "=" << struct_element_value<I, T>(value))
											 : ss),
					 ...);
				};

				f(std::make_index_sequence<struct_size<T>()>());
			}

			template <aquarius::detail::string_literal Name, typename T>
			constexpr void contains()
			{
				constexpr static auto member_name = std::decay_t<T>::member_name();

				constexpr static auto size = struct_size<std::decay_t<T>>();

				constexpr auto f = [&]<std::size_t... I>(std::index_sequence<I...>) -> bool
				{ return ((aquarius::detail::bind_param<Name>::value == std::get<I>(member_name)) || ...); };

				constexpr static auto result = " is not exsist"sv;

				constexpr auto boolean_res = f(std::make_index_sequence<size>{});

				//static_assert(boolean_res, concat_v<aquarius::detail::bind_param<Name>::value, result>);
				static_assert(boolean_res);
			}

			template <typename T, aquarius::detail::string_literal... Name>
			inline constexpr void check_table_name()
			{
				constexpr auto size = sizeof...(Name);

				constexpr auto f = [&]<std::size_t... I>(std::index_sequence<I...>) { (contains<Name, T>(), ...); };

				return f(std::make_index_sequence<size>{});
			}

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
					{ ((complete_sql << struct_element_name<I, T>() << (size != I + 1 ? "," : "")), ...); };

					type_f(std::make_index_sequence<size>{});

					complete_sql << ") values(";

					auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
					{ ((complete_sql << struct_element_value<I, T>(value) << (size != I + 1 ? "," : "")), ...); };

					f(std::make_index_sequence<size>{});

					complete_sql << ")";

					return complete_sql.str();
				}

				std::stringstream complete_sql;
			};

			template <typename T, aquarius::detail::string_literal... Name>
			struct select_view
			{
				auto operator()(const T& value) -> std::string
				{
					static_assert(sizeof...(Name) <= struct_size<T>(), "too many fields");

					check_table_name<T, Name...>();

					complete_sql.str("");

					constexpr auto struct_name = aquarius::detail::struct_name<std::remove_cvref_t<T>>();

					complete_sql << "select";

					int start = 0;

					if constexpr (sizeof...(Name) == 0)
					{
						complete_sql << " *";
					}
					else
					{
						auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
						{
							((complete_sql << (start++ == 0 ? " " : ",") << aquarius::detail::bind_param<Name>::value),
							 ...);
						};

						f(std::make_index_sequence<sizeof...(Name)>{});
					}

					complete_sql << " from " << struct_name;

					detail::make_sql_condition(complete_sql, value);

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

					detail::make_sql_condition(complete_sql, value);

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

					detail::make_value_update(complete_sql, value);

					detail::make_sql_condition(complete_sql, value, " set ");

					return complete_sql.str();
				}

				std::stringstream complete_sql;
			};
		} // namespace detail
		static detail::insert_view insert;
		static detail::remove_view remove;
		static detail::update_view update;

		template <typename T, aquarius::detail::string_literal... Name>
		static detail::select_view<T, Name...> select;
	} // namespace tbl
} // namespace aquarius