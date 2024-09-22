#pragma once
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{
	class basic_connect
	{
	public:
		virtual void send_packet(flex_buffer_t&&) = 0;

		virtual std::size_t uuid() const = 0;
	};
} // namespace aquarius