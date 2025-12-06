#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>

namespace aquarius
{
	template <typename Timer, typename Executor = boost::asio::any_io_executor>
	class timer
	{
		using duration = typename Timer::duration;

	public:
		explicit timer(io_context& ctx, duration timeout)
			: timer(ctx.get_executor(), timeout)
		{}

		timer(const Executor& ex, duration timeout)
			: executor_(ex)
			, timer_(executor_)
			, timeout_(timeout)
		{}

	public:
		template <typename Func>
		auto async_wait(Func&& f) -> awaitable<void>
		{
			for (;;)
			{
				timer_.expires_after(timeout_);

				co_await timer_.async_wait(use_awaitable);

				std::forward<Func>(f)();
			}
		}

		void cancel()
		{
			error_code ec;

			timer_.cancel(ec);
		}

	private:
		Executor executor_;

		Timer timer_;

		duration timeout_;
	};
} // namespace aquarius