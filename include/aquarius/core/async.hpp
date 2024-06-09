#pragma once
#include <aquarius/core/async/async_pool.hpp>

namespace aquarius
{
	template<typename _Func>
	inline void post(_Func&& f)
	{
		async_pool::instance().post(std::forward<_Func>(f));
	}
}
