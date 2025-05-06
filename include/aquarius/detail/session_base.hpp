#pragma once
#include <aquarius/detail/flex_buffer.hpp>

namespace aquarius
{
	class session_base
	{
	public:
		virtual void send_packet(flex_buffer_t&&) = 0;
	};
} // namespace aquarius