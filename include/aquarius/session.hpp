#pragma once
#include <aquarius/tcp/async_io_service.hpp>
#include <aquarius/detail/basic_session.hpp>
#include <aquarius/ssl/no_ssl_protocol.hpp>

namespace aquarius
{
	using async_tcp_session = basic_session<tcp::async_io_service<ssl::no_ssl_protocol>>;

#ifdef AQUARIUS_ENABLE_SSL
	template<typename Executor = boost::asio::any_io_executor>
	using async_tcp_ssl_session = basic_session<async_io_service<boost::asio::ip::tcp, Executor, true, ssl>>;
#endif
}