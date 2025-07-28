#pragma once
#include <cstring>

namespace aquarius
{
	namespace detail
	{
		inline static std::size_t client_pool_size = 4;
	} // namespace detail

	inline void set_client_pool_size(std::size_t pool_size)
	{
		detail::client_pool_size = pool_size;
	}

	inline std::size_t get_client_pool_size()
	{
		return detail::client_pool_size;
	}
} // namespace aquarius