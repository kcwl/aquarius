#pragma once
#include <aquarius/io_context.hpp>

namespace aquarius
{
	template <typename Timer, typename Executor = boost::asio::any_io_executor>
	class timer_adaptor
	{
	public:
		explicit timer_adaptor(io_context& ctx)
			: timer_adaptor(ctx.get_executor())
		{}

		timer_adaptor(const Executor& ex)
			: executor_(ex)
			, timer_(executor_)
		{

		}

	public:
		template <typename Rep, typename Ratio, typename Func>
		auto async_wait(std::chrono::duration<Rep, Ratio> dura, Func&& f) -> awaitable<void>
		{
			for (;;)
			{
				timer_.expires_after(dura);

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
	};
} // namespace aquarius