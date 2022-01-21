#pragma once
#include "filed.hpp"

namespace  aquarius
{
	namespace tcp
	{
		template<typename T, typename Off, typename ProtoType, typename Result>
		struct header_value
		{
			T type_;
			Off length_;
			ProtoType proto_;
			Result result_;
		};

		using header_fields = basic_fileds<std::allocator<header_value<int, int, int, int>>>;
	}
}