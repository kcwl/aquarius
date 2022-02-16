#pragma once
#include <cstdint>
#include "filed.hpp"

namespace  aquarius
{
	namespace tcp
	{
		struct header_value
		{
			uint32_t proto_;
			uint32_t length_;
			uint32_t result_;

			bool operator==(const header_value& rhs)
			{
				return  this->proto_ == rhs.proto_ && this->length_ == rhs.length_ && this->result_ == rhs.result_;
			}
		};

		using header_fields = basic_fileds<std::allocator<header_value>>;
	}
}