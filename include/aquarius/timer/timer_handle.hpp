#pragma once


namespace aquarius
{
	class timer_handle
	{
	public:
		virtual void handle_update() = 0;
	};
}