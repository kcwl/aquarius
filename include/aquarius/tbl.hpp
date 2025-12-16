#pragma once
#include <aquarius/tbl/engine/field.hpp>
#include <aquarius/tbl/engine/generate_view.hpp>
#include <aquarius/tbl/engine/integer.hpp>
#include <aquarius/tbl/engine/key.hpp>
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