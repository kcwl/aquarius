#pragma once
#include <aquarius/flex_buffer.hpp>

namespace aquarius
{
	class basic_connector
	{
	public:
		virtual void write(flex_buffer_t&&) = 0;

		virtual void shut_down() = 0;

		virtual flex_buffer_t& get_read_buffer() = 0;

		virtual std::size_t uuid() = 0;

		virtual std::string remote_address() = 0;

		virtual uint16_t remote_port() = 0;
	};
} // namespace aquarius
