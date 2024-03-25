#pragma once
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <aquarius/client.hpp>
#include <aquarius/connect.hpp>
#include <aquarius/context.hpp>
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>
#include <aquarius/server.hpp>
#include <aquarius/service.hpp>

namespace aquarius
{
	using tcp_server = server<connect<tcp, conn_mode::server, ssl_mode::ssl>>;

	using no_ssl_tcp_server = server<connect<tcp, conn_mode::server, ssl_mode::nossl>>;

	using http_server = server<connect<http, conn_mode::server, ssl_mode::nossl>>;

	using https_server = server<connect<http, conn_mode::server, ssl_mode::ssl>>;

	using tcp_client = client<connect<tcp, conn_mode::client, ssl_mode::ssl>>;

	using no_ssl_tcp_client = client<connect<tcp, conn_mode::client, ssl_mode::nossl>>;

	template <typename _Message>
	inline void broadcast(_Message&& msg)
	{
		flex_buffer_t fs{};
		msg.to_binary(fs);

		router_session::instance().broadcast(std::move(fs));
	}

	template <typename _Message, typename _Func>
	inline void broadcast(_Message&& msg, _Func&& f)
	{
		flex_buffer_t fs{};
		msg.to_binary(fs);

		router_session::instance().broadcast(std::move(fs), std::forward<_Func>(f));
	}
} // namespace aquarius