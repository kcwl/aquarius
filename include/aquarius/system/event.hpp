#pragma once

namespace aquarius
{
	namespace detail
	{
		class event_call
		{
		public:
			virtual void on_accept() = 0;

			virtual void on_close() = 0;

			virtual void on_timeout() = 0;
		};
	} // namespace detail
} // namespace aquarius