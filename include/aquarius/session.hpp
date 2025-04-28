#pragma once
#include <aquarius/detail/async_io.hpp>
#include <aquarius/detail/sync_io.hpp>
#include <aquarius/detail/basic_session.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace aquarius
{
	template<typename Executor = boost::asio::any_io_executor>
	using sync_tcp_session = basic_session<sync_io_service<boost::asio::ip::tcp, Executor>>;

	template<typename Executor = boost::asio::any_io_executor>
	using async_tcp_session = basic_session<async_io_service<boost::asio::ip::tcp, Executor>>;

#ifdef AQUARIUS_ENABLE_SSL
	template<typename Executor = boost::asio::any_io_executor>
	using sync_tcp_ssl_session = basic_session<sync_ssl_io_service<boost::asio::ip::tcp, Executor>>;

	template<typename Executor = boost::asio::any_io_executor>
	using async_tcp_ssl_session = basic_session<async_ssl_io_service<true, boost::asio::ip::tcp, Executor>>;
#endif
}