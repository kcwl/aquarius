#pragma once
#include <aquarius/basic_server.hpp>
#include <aquarius/basic_session.hpp>
#include <aquarius/basic_ssl_session_service.hpp>
#include <aquarius/detail/protocol.hpp>
#include <aquarius/ssl/ssl.hpp>
#include <aquarius/tcp/async_acceptor.hpp>

#ifdef AQUARIUS_ENABLE_SSL
namespace aquarius
{
	namespace ssl
	{
		using session_service = basic_ssl_session_service<true, boost::asio::ip::tcp>;

		using tcp_ssl_session = basic_session<session_service, package_processor<protocol::tcp>>;

		using tcp_server = basic_server<tcp::async_acceptor, tcp_ssl_session>;
	} // namespace ssl
} // namespace aquarius
#endif