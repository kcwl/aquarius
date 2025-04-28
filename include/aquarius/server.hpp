#pragma once
#include <aquarius/detail/async_io.hpp>
#include <aquarius/detail/async_acceptor.hpp>
#include <aquarius/detail/basic_server.hpp>
#include <aquarius/detail/sync_io.hpp>
#include <boost/asio/any_io_executor.hpp>

namespace aquarius
{
	template <typename Protocol, typename Connector, typename Executor = boost::asio::any_io_executor>
	using async_server = basic_server<async_acceptor<Protocol, Executor>, Connector>;

	template <typename Protocol, typename Connector, typename Executor = boost::asio::any_io_executor>
	using sync_server = basic_server<sync_io_acceptor<Protocol, Executor>, Connector>;

} // namespace aquarius
