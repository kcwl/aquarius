#pragma once
#include <aquarius/io_context.hpp>

namespace aquarius
{
	template <typename Timer>
	class timer_adaptor
	{
	public:
		explicit timer_adaptor(io_context& ctx)
			: context_(ctx)
			, timer_(context_)
		{}

	public:
		template <typename Rep, typename Ratio, typename Func>
		auto async_wait(std::chrono::duration<Rep, Ratio> dura, Func&& f) -> awaitable<void>
		{
			for (;;)
			{
				timer_.expires_after(dura);

				co_await timer_.async_wait(use_awaitable);

				if (!f)
					co_return;

				std::forward<Func>(f)();
			}
		}

		void cancel()
		{
			error_code ec;

			timer_.cancel(ec);
		}

	private:
		io_context& context_;

		Timer timer_;
	};
} // namespace aquarius