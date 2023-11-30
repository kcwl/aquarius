#pragma once
#include <string>

namespace sqlpro
{
	namespace detail
	{
		template<class T>
		struct indentify {};

		constexpr auto mysql_type(indentify<std::string>)
		{
			return "varchar(255)";
		}

		constexpr auto mysql_type(indentify<int>)
		{
			return "int(11)";
		}

		template<class T>
		constexpr auto mysql_type(indentify<T>)
		{
			return mysql_type(indentify<std::string>{});
		}

		constexpr auto mysql_type(indentify<uint64_t>)
		{
			return "bigint";
		}

		constexpr auto mysql_type(indentify<bool>)
		{
			return "int";
		}

		constexpr auto mysql_type(indentify<time_t>)
		{
			return "bigint";
		}

		constexpr auto mysql_type(indentify<std::byte>)
		{
			return "bit";
		}
	}
}