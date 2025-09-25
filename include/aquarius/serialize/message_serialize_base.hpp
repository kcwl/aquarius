#pragma once
#include <aquarius/detail/flex_buffer.hpp>

namespace aquarius
{
	namespace virgo
	{
		class message_serialize_base
		{
		public:
			message_serialize_base() = default;

			virtual ~message_serialize_base() = default;

		public:
			virtual void serialize(detail::flex_buffer<char>& buffer) = 0;

			virtual void deserialize(detail::flex_buffer<char>& buffer) = 0;
		};
	} // namespace virgo
} // namespace aquarius
