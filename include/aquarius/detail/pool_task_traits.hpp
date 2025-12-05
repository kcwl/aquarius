#pragma once
#include <aquarius/error_code.hpp>
#include <aquarius/asio.hpp>

namespace aquarius
{
	template <typename T, typename Core>
	concept enable_pool_task = requires {
		typename T::return_type;

		{ std::declval<T>()(std::shared_ptr<Core>{}) } -> std::same_as<awaitable<typename T::return_type>>;
	};
} // namespace aquarius