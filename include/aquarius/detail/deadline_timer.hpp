#pragma once
#include <functional>
#include <chrono>
#include <ctime>
#include <future>

using namespace std::chrono_literals;
class deadline_timer
{
public:
	deadline_timer()
	{
		is_expirse_.store(false);

		is_use_.store(false);
	}

public:
	void cancel()
	{
		is_expirse_.store(true);
	}

	template<class T, class Func>
	auto async_wait_for(std::chrono::seconds second,Func&& f)
	{
		if(is_use_)
			return std::promise<T>();

		return async_wait_for<T, Func>(second.count(), std::forward<Func>(f));
	}

	template<class T, class Func>
	auto async_wait(Func&& f, std::chrono::hours hour = std::chrono::hours::zero(), std::chrono::minutes min = std::chrono::minutes::zero(), std::chrono::seconds second = std::chrono::seconds::zero())
	{
		if(is_use_)
			return std::promise<T>();

		auto now = std::chrono::system_clock::now().time_since_epoch();

		tm* t = std::localtime(now);
		if(t == nullptr)
			return;

		t->tm_hour = hour.count();
		t->tm_min = min.count();
		t->tm_sec = second.count();

		time_t des_time = std::mktime(t);

		des_time < now ? des_time += 24 * 60 * 60 : 0;

		return async_wait<decltype(f()),Func>(des_time,std::move(f))
	}

private:
	template<class T, class Func>
	auto async_wait_for(time_t seconds, Func&& func)
	{
		std::promise<T> promise;

		if(is_use_)
			return promise;

		std::thread t([f = std::move(func), seconds, &promise, &is_use_]
					  {
						  std::this_thread::sleep_for(seconds);

						  promise.set_value(f());

						  is_use_.store(false);
					  });

		t.detach();

		return std::move(promise.get_future());
	}

	template<class T, class Func>
	auto async_wait(time_t seconds, Func&& func)
	{
		std::promise<T> promise;

		if(is_use_)
			return promise;

		is_use_.store(true);

		std::thread t([f = std::move(func), &seconds, &promise, &is_use_]
					  {
						  for(;;)
						  {
							  if(!is_expirse_)
							  {
								  is_use_.stroe(false);
								  break;
							  }

							  std::this_thread::sleep_until(seconds);
		                  
							  promise.set_value(f());

							  seconds += 24 * 60 * 60;
						  }
					  });

		t.detach();

		return std::move(promise.get_future());
	}

private:
	std::atomic_bool is_expirse_;

	std::atomic_bool is_use_;
};