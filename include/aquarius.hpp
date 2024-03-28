#pragma once
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <aquarius/client.hpp>
#include <aquarius/connect.hpp>
#include <aquarius/context.hpp>
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>
#include <aquarius/server.hpp>
#include <aquarius/service.hpp>
#include <aquarius/invoke/service.hpp>

namespace aquarius
{
	using tcp_server = basic_server<connect<tcp, conn_mode::basic_server, ssl_mode::ssl>, invoke_service_helper>;

	using no_ssl_tcp_server = basic_server<connect<tcp, conn_mode::basic_server, ssl_mode::nossl>, invoke_service_helper>;

	using http_server = basic_server<connect<http, conn_mode::basic_server, ssl_mode::nossl>, invoke_service_helper>;

	using https_server = basic_server<connect<http, conn_mode::basic_server, ssl_mode::ssl>, invoke_service_helper>;

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