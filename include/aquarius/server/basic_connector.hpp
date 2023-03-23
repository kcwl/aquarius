#pragma once
#include <aquarius/core/flex_buffer.hpp>

namespace aquarius
{
	namespace srv
	{
		class basic_connector
		{
		public:
			virtual void write(core::flex_buffer_t&&) = 0;

			virtual void shut_down() = 0;

			virtual core::flex_buffer_t& get_read_buffer() = 0;

			virtual uuids::uuid uid() = 0;
		};
	}
}