#pragma once
#include <aquarius/async_acceptor.hpp>
#include <aquarius/detail/basic_server.hpp>
#include <boost/asio/any_io_executor.hpp>

namespace aquarius
{
	template <typename Protocol, typename Connector, typename Executor = boost::asio::any_io_executor>
	using async_server = basic_server<async_acceptor<Protocol, Executor>, Connector>;
} // namespace aquarius
