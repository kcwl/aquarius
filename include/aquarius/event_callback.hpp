#pragma once

namespace aquarius
{
	class event_callback
	{
	public:
		virtual void on_accept() = 0;

		virtual void on_close() = 0;

		virtual void on_timeout() = 0;
	};
}