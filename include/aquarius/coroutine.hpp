#pragma once
#include <boost/asio/use_future.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/experimental/coro.hpp>
#include <boost/asio/cancel_after.hpp>

namespace aquarius
{
	using boost::asio::awaitable;

	using boost::asio::co_spawn;

	using boost::asio::detached;

	using boost::asio::post;

	using boost::asio::use_awaitable;

	using boost::asio::use_future;

	using boost::asio::experimental::coro;

	using boost::asio::cancel_after;
} // namespace aquarius