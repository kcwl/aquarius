#pragma once
#include <aquarius/core/async.hpp>
#include <future>
#include <memory>

namespace aquarius
{
	class async_handle
	{
	public:
		template <typename _Result, typename _Func, typename... _Args>
		std::future<_Result> post(_Func&& f, _Args&&... args)
		{
			auto task = std::make_shared<std::packaged_task<_Result(_Args...)>>(std::forward<_Func>(f));

			auto future = task->get_future();

			aquarius::post([task] { (*task)(std::forward<_Args>(args)...); });

			return future;
		}
	};
} // namespace aquarius