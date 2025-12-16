#pragma once
#include <aquarius/tbl/field.hpp>
#include <aquarius/tbl/generate_view.hpp>
#include <aquarius/tbl/integer.hpp>
#include <aquarius/tbl/key.hpp>
#include <aquarius/tbl/sql_pool.hpp>

namespace aquarius
{
	namespace tbl
	{
		struct _float
		{
			constexpr static auto name = "float"sv;
		};

		struct _double
		{
			constexpr static auto name = "_double"sv;
		};

		using string = std::string;
	} // namespace tbl
} // namespace aquarius