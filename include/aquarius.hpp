#pragma once
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <aquarius/channel.hpp>
#include <aquarius/client.hpp>
#include <aquarius/connect.hpp>
#include <aquarius/context.hpp>
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>
#include <aquarius/server.hpp>
#include <aquarius/service.hpp>
#include <aquarius/connect/ssl.hpp>
#include <aquarius/connect/no_ssl.hpp>
#include <aquarius/core/ini_tree.hpp>
#include <aquarius/core/crc.hpp>
#include <aquarius/core/singleton.hpp>
#include <aquarius/core/core.hpp>

namespace aquarius
{
	using tcp_server = basic_server<ssl_tcp_server_connect, channel::publisher>;

	using no_ssl_tcp_server = basic_server<no_ssl_tcp_server_connect, channel::publisher>;

	//using http_server = basic_server<connect<http, conn_mode::basic_server, ssl_mode::nossl>, channel::publisher>;

	//using https_server = basic_server<connect<http, conn_mode::basic_server, ssl_mode::ssl>, channel::publisher>;

	using tcp_client = client<ssl_tcp_client_connect>;

	using no_ssl_tcp_client = client<no_ssl_tcp_client_connect>;

	template <typename _Message>
	inline void broadcast(_Message&& msg)
	{
		flex_buffer_t fs{};
		error_code ec{};
		msg.to_binary(fs, ec);

		session_manager::instance().broadcast(std::move(fs));
	}

	template <typename _Message, typename _Func>
	inline void broadcast(_Message&& msg, _Func&& f)
	{
		flex_buffer_t fs{};
		error_code ec{};
		msg.to_binary(fs, ec);

		session_manager::instance().broadcast(std::move(fs), std::forward<_Func>(f));
	}
} // namespace aquarius