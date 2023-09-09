#pragma once
#include "detail/deadline_timer.hpp"
#include "detail/singleton.hpp"

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <functional>
#include <mutex>
#include <unordered_map>

namespace aquarius
{
	class timer_queue : public detail::singleton<timer_queue>
	{
	public:
		enum class timer_category
		{
			clock,
			duration
		};

		struct timer_info
		{
			timer_category cate_;
			std::function<void()> callback_;
			std::chrono::milliseconds duration_;
			std::shared_ptr<detail::deadline_timer> timer_ptr_;
		};

	public:
		timer_queue() = default;

	public:
		template <typename _Func>
		std::size_t regist(_Func&& f, timer_category cate, std::chrono::milliseconds ms)
		{
			std::lock_guard lk(mutex_);

			timer_info info{};
			info.cate_ = cate;
			info.callback_ = std::forward<_Func>(f);
			info.duration_ = ms;

			boost::uuids::random_generator_mt19937 generator{};

			auto uid = boost::uuids::hash_value(generator());

			timers_.insert({ uid, info });

			return uid;
		}

		void exec(boost::asio::io_service& ios)
		{
			std::lock_guard lk(mutex_);

			for (auto& timer : timers_)
			{
				timer.second.timer_ptr_ = std::make_shared<detail::deadline_timer>(ios);

				exec_task(timer.second);
			}
		}

		void stop()
		{
			std::lock_guard lk(mutex_);

			for (auto& timer : timers_)
			{
				if (!timer.second.timer_ptr_)
					continue;

				timer.second.timer_ptr_->cancel();
			}
		}

	private:
		void exec_task(timer_info& info)
		{
			info.timer_ptr_->expires_from_now(info.duration_);
			info.timer_ptr_->async_wait(
				[&](const boost::system::error_code& ec)
				{
					if (ec == boost::asio::error::operation_aborted)
						return;

					if (info.callback_)
						info.callback_();

					if (info.cate_ == timer_category::clock)
					{
						info.cate_ = timer_category::duration;
						info.duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::days());
					}

					return exec_task(info);
				});
		}

	private:
		std::unordered_map<std::size_t, timer_info> timers_;

		std::mutex mutex_;
	};
} // namespace aquarius