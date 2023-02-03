#pragma once


namespace aquarius
{
	namespace impl
	{
		class basic_connector
		{
		public:
			virtual void write(flex_buffer_t&&) = 0;

			virtual void shut_down() = 0;

			virtual flex_buffer_t& get_read_buffer() = 0;
		};
	}
}