#pragma once
#include <aquarius/concepts.hpp>
#include <aquarius/detail/struct_name.hpp>
#include <aquarius/tbl/add_string.hpp>
#include <boost/pfr.hpp>
#include <sstream>

namespace aquarius
{
	class insert_view
	{
	public:
		template <typename T>
		insert_view& operator()(T&& value)
		{
			complete_sql_.str("");

			constexpr auto struct_name = aquarius::detail::struct_name<std::remove_cvref_t<T>>();

			complete_sql_ << "insert into " << struct_name << "(";

			using type = std::remove_cvref_t<T>;

			constexpr static auto size = boost::pfr::tuple_size_v<type>;

			auto type_f = [&]<std::size_t... I>(std::index_sequence<I...>)
			{ ((complete_sql_ << boost::pfr::get_name<I, type>() << (size != I + 1 ? "," : "")), ...); };

			type_f(std::make_index_sequence<size>{});

			complete_sql_ << ") values(";

			auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
			{ ((complete_sql_ << add_string(boost::pfr::get<I, type>(value)) << (size != I + 1 ? "," : "")), ...); };

			f(std::make_index_sequence<size>{});

			complete_sql_ << ")";

			return *this;
		}

		operator std::string() const
		{
			return complete_sql_.str();
		}

	private:
		std::stringstream complete_sql_;
	};

	inline static insert_view insert_v;
} // namespace aquarius