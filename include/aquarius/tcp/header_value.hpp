#pragma once
#include "filed.hpp"

namespace  aquarius
{
	namespace tcp
	{
		template<typename T, typename Off, typename ProtoType, typename Result>
		struct value_type
		{
			T type_;
			Off length_;
			ProtoType proto_;
			Result result_;
		};

		using header_fields = basic_fileds<std::allocator<value_type<int, int, int, int>>>;
	}
}