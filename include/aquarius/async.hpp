#pragma once
#include <aquarius/concepts/async.hpp>
#include <aquarius/detail/io_service_pool.hpp>
#include <aquarius/client_pool.hpp>

namespace aquarius
{
	template <client_t T>
	std::shared_ptr<T> async_generator(int reconnect_times = 3, std::chrono::steady_clock::duration timeout = 100ms)
	{
		static detail::io_service_pool client_pool(get_client_pool_size());

		return std::make_shared<T>(client_pool.get_io_service(), reconnect_times, timeout);
	}
} // namespace aquarius