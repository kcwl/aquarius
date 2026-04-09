#pragma once
#include <aquarius/tbl/concepts.hpp>
#include <chrono>
#include <ostream>
#include <sstream>
#include <string>

namespace aquarius
{
	namespace mysql
	{
		using string = std::string;
		using uint64_t = std::uint64_t;
		using int64_t = std::int64_t;
		using blob = std::vector<uint8_t>;
		using date = boost::mysql::date;
		using datetime = boost::mysql::datetime;
		using time = boost::mysql::time;
	} // namespace mysql

	template <typename T, typename U>
	requires(std::same_as<T, U> && !push_t<T> && !push_t<U>)
	T type_cast(U value)
	{
		return value;
	}

	template <push_t T, iterator_t U>
	T type_cast(U view)
	{
		T b{};

		std::copy(view.begin(), view.end(), std::back_inserter(b));

		return b;
	}
} // namespace aquarius