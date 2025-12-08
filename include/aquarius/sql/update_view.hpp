#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/detail/struct_name.hpp>
#include <aquarius/sql/concepts.hpp>
#include <boost/pfr.hpp>
#include <sstream>

namespace aquarius
{
	namespace sql
	{
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
	} // namespace sql
} // namespace aquarius