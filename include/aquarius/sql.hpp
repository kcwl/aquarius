#pragma once
#include <string_view>
#include <format>

namespace aquarius
{
	namespace sql
	{
		namespace detail
		{
			template <typename _Ty>
			inline constexpr std::string_view name()
			{
				using namespace std::string_view_literals;

#ifndef __linux
				constexpr std::string_view name = __FUNCSIG__ ""sv;

				constexpr auto left_bracket = name.find_last_of("<");

				constexpr auto end_bracket = name.find_last_of(">");

				constexpr auto temp_name = name.substr(left_bracket + 1, end_bracket - left_bracket - 1);

				constexpr auto start = name.find_last_of(" ");

				if constexpr (start == std::string_view::npos)
				{
					return temp_name;
				}
				else
				{
					return name.substr(start + 1, end_bracket - start - 1);
				}
#else
				constexpr std::string_view name = __PRETTY_FUNCTION__;

				constexpr auto left_bracket = name.find_last_of("[");
				constexpr auto right_bracket = name.find_last_of("]");
				constexpr auto name_in_bracket = name.substr(left_bracket + 1, right_bracket - left_bracket - 1);

				constexpr auto left_equ = name_in_bracket.find_first_of("=");
				constexpr auto right_f = name_in_bracket.find_first_of(";");

				constexpr auto first_name = name_in_bracket.substr(left_equ + 2, right_f - left_equ - 2);

				constexpr auto sp = first_name.find_last_of(":");

				if constexpr (sp == std::string_view::npos)
				{
					return first_name;
				}
				else
				{
					return first_name.substr(sp + 1);
				}
#endif
			}

			template <typename Adaptor, typename... Args>
			struct sql_partial
			{
				std::tuple<Args...> args;

				sql_partial(Args... args)
					: args(std::move(args)...)
				{}

				template <typename T>
				constexpr auto operator()(T&& t) const
				{
					auto forwarder = [&t](const auto&... _args) { return Adaptor{}(std::forward<T>(t), _args...); };

					return std::apply(forwarder, args);
				}
			};

			struct sql_adaptor_closure
			{
				template <typename Self, typename T>
				friend constexpr auto operator|(T&& t, Self&& self)
				{
					return std::forward<Self>(self)(std::forward<T>(t));
				}
			};

			template <typename T>
			struct sql_adaptor : public sql_adaptor_closure
			{
				template <typename... Args>
				constexpr auto operator()(Args&&... args) const
				{
					return sql_partial<T, std::decay_t<Args>...>{ std::forward<Args>(args)... };
				}
			};
		} // namespace detail

		template <typename T>
		class select_view
		{
		public:
			constexpr select_view() = default;

		public:
			constexpr auto result() -> std::string
			{
				constexpr static auto struct_name = detail::name<T>();

				return std::format("select * from {}", struct_name.data());
			}
		};

		template <typename T>
		class insert_view
		{
		public:
			insert_view(T value)
			{
				constexpr static auto struct_name = detail::name<T>();

				constexpr auto size = boost::pfr::tuple_size_v<T>;

				auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
				{ return std::make_tuple((boost::pfr::get<I, T>(value), ...)); };

				complete_sql_ =
					std::format("insert into {} values {}", struct_name, f(std::make_index_sequence<size>{}));
			}

		public:
			auto result() -> std::string
			{
				return complete_sql_;
			}

		private:
			std::string complete_sql_;
		};

		template <typename T>
		class remove_view
		{
		public:
			constexpr remove_view() = default;

		public:
			constexpr auto result() -> std::string
			{
				constexpr static auto struct_name = detail::name<T>();

				return std::format("delete from {}", struct_name);
			}
		};

		template <typename T>
		class update_view
		{
		public:
			update_view(T value)
			{
				constexpr static auto struct_name = detail::name<T>();

				constexpr auto update_sql = std::format("update {} set ", struct_name);

				ss_ << update_sql;

				constexpr auto size = boost::pfr::tuple_size_v<T>;

				boost::pfr::for_each_field_with_name(
					value, [&](auto name, auto value, std::size_t index)
					{ ss_ << name.data() << "=" << add_string_format(value) << (index != size - 1 ? " and " : ""); });
			}

		public:
			auto result() -> std::string
			{
				return ss_.str();
			}

		private:
			std::stringstream ss_;
		};

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

		struct _insert
		{
			template <typename Ranges>
			constexpr auto operator()(Ranges&& ranges) const
			{
				return insert_view<Ranges>(std::forward<Ranges>(ranges));
			}
		};

		inline constexpr _insert insert;

		struct _remove
		{
			template <typename Ranges>
			constexpr auto operator()(Ranges&& ranges) const
			{
				return remove_view<Ranges>(std::forward<Ranges>(ranges));
			}
		};

		inline constexpr _remove remove;

		struct _update
		{
			template <typename Ranges>
			constexpr auto operator()(Ranges&& ranges) const
			{
				return update_view<Ranges>(std::forward<Ranges>(ranges));
			}
		};

		inline constexpr _update update;

		template <typename T>
		class distinct_view
		{
		public:
			distinct_view(T view)
			{
				auto sql = view.result();

				auto pos = sql.find_first_of(" ");

				if (pos != std::string_view::npos)
				{
					sql_ = sql.substr(0, pos) + " distinct " + sql.substr(pos + 1);
				}
			}

		public:
			constexpr auto result() -> std::string
			{
				return sql_;
			}

		private:
			std::string sql_;
		};

		struct _distinct : detail::sql_adaptor<_distinct>
		{
			template <typename T>
			constexpr auto operator()(T&& v) const
			{
				return distinct_view<T>(std::forward<T>(v));
			}

			using detail::sql_adaptor<_distinct>::operator();
		};

		inline constexpr _distinct distinct;

		template <typename T>
		class where_view
		{
		public:
			where_view(T ranges, const std::string& sql)
				: sql_(ranges.result() + " where " + sql)
			{}

			template <typename Attr>
			where_view(Attr&& attr)
				: sql_(attr)
			{}

		public:
			auto result() -> std::string
			{
				return sql_;
			}

		private:
			std::string sql_;
		};

		struct _where : detail::sql_adaptor<_where>
		{
			template <typename T, typename Attr>
			constexpr auto operator()(T&& v, Attr&& attr) const
			{
				return where_view<T>(std::forward<T>(v), std::forward<Attr>(attr));
			}

			using detail::sql_adaptor<_where>::operator();
		};

		inline constexpr _where where;

		template <typename T>
		class order_view
		{
		public:
			template <typename Attr>
			order_view(T v, Attr&&)
			{
				sql_ = v.result() + " order by " + detail::name<Attr>();
			}

		public:
			auto result() -> std::string
			{
				return sql_;
			}

		private:
			std::string sql_;
		};

		struct _order
		{
			template <typename T, typename Attr>
			constexpr auto operator()(T&& v, Attr&& attr) const
			{
				return order_view<T>(std::forward<T>(v), std::forward<Attr>(attr));
			}
		};

		inline constexpr _order order;

		template <typename T>
		class limit_view
		{
		public:
			limit_view(T ranges, const std::string& limit)
			{
				sql_ = ranges.result() + " limit " + limit;
			}

		public:
			auto result() -> std::string
			{
				return sql_;
			}

		private:
			std::string sql_;
		};

		struct _limit : detail::sql_adaptor<_limit>
		{
			template <typename Ranges>
			constexpr auto operator()(Ranges&& ranges, int limit) const
			{
				return limit_view<Ranges>(std::forward<Ranges>(ranges), std::to_string(limit));
			}

			using detail::sql_adaptor<_limit>::operator();
		};

		inline constexpr _limit limit;

		template <typename T, typename Attr>
		class and_view
		{
			and_view(T ranges, Attr attr)
				: sql_(ranges.result() + " and " + attr)
			{}

		public:
			auto result() -> std::string
			{
				return sql_;
			}

		private:
			std::string sql_;
		};

		struct _and : detail::sql_adaptor<_where>
		{
			template <typename T, typename Attr>
			constexpr auto operator()(T&& v, Attr attr) const
			{
				return and_view<T, Attr>(std::forward<T>(v), std::move(attr));
			}

			using detail::sql_adaptor<_where>::operator();
		};

		inline constexpr _and and_;

		template <typename T, typename Attr>
		class or_view
		{
			or_view(T ranges, Attr attr)
			{
				auto name = detail::name<Attr>();

				sql_ = " or " + name;
			}

		public:
			auto result() -> std::string
			{
				return sql_;
			}

		private:
			std::string sql_;
		};

		struct _or : detail::sql_adaptor<_where>
		{
			template <typename T, typename Attr>
			constexpr auto operator()(T&& v, Attr attr) const
			{
				return or_view<T, Attr>(std::forward<T>(v), std::move(attr));
			}
		};

		inline constexpr _or or_;
	} // namespace sql



} // namespace aquarius

#define sql_where(content) aquarius::sql::where(#content) 
#define sql_and(content) aquarius::sql::and_(#content) 
#define sql_or(content) aquarius::sql::or_(#content) 