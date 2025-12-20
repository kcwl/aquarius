#pragma once
#include <aquarius/tbl/basic_datatype.hpp>
#include <aquarius/tbl/engine/bool.hpp>
#include <aquarius/tbl/engine/field.hpp>
#include <aquarius/tbl/engine/float.hpp>
#include <aquarius/tbl/engine/generate_view.hpp>
#include <aquarius/tbl/engine/integer.hpp>
#include <aquarius/tbl/engine/key.hpp>
#include <aquarius/tbl/engine/text.hpp>
#include <aquarius/tbl/engine/varchar.hpp>
#include <aquarius/tbl/schedule.hpp>
#include <aquarius/tbl/sql_connector.hpp>

namespace aquarius
{
	namespace tbl
	{
		using int1 = basic_datatype<uint8_t, integer>;
		using int2 = basic_datatype<uint16_t, integer>;
		using int3 = basic_datatype<int32_t, integer>;
		using int4 = basic_datatype<uint32_t, integer>;
		using int8 = basic_datatype<uint64_t, integer>;

		using tinytext = basic_text<uint8_t, vtext>;
		using text = basic_text<uint16_t, vtext>;
		using mediumtext = basic_text<uint32_t, vtext>;
		using longtext = basic_text<uint64_t, vtext>;

		using tinyblob = basic_text<uint8_t, blob_impl>;
		using blob = basic_text<uint16_t, blob_impl>;
		using mediumblob = basic_text<uint32_t, blob_impl>;
		using longblob = basic_text<uint64_t, blob_impl>;

		using boolean = basic_datatype<bool, _bool>;
		using floatable = basic_datatype<float, _float>;
		using doublable = basic_datatype<double, _double>;

		template <auto N>
		using dchar = basic_char<N>;

		using varchar = basic_datatype<std::string, _varchar>;

		using nvarchar = basic_datatype<std::wstring, _nvarchar>;
	} // namespace tbl
} // namespace aquarius