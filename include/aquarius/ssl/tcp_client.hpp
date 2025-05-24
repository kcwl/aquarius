#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/basic_session.hpp>
#include <aquarius/basic_ssl_session_service.hpp>
#include <aquarius/detail/protocol.hpp>
#include <aquarius/ssl/ssl.hpp>

#ifdef AQUARIUS_ENABLE_SSL
namespace aquarius
{
	namespace ssl
	{
		using tcp_sesssion_service = basic_ssl_session_service<false, boost::asio::ip::tcp>;

		using tcp_session = basic_session<tcp_sesssion_service, package_processor<aquarius::protocol::tcp>>;

		using tcp_client = basic_client<boost::asio::ip::tcp, tcp_session>;
	} // namespace ssl
} // namespace aquarius
#endif