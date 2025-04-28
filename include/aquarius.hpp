#pragma once
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <aquarius/client.hpp>
#include <aquarius/context.hpp>
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>
#include <aquarius/server.hpp>
#include <aquarius/core/ini_tree.hpp>
#include <aquarius/core/crc.hpp>
#include <aquarius/core/singleton.hpp>
#include <aquarius/core/core.hpp>
#include <aquarius/session.hpp>

namespace aquarius
{
	//using tcp_server = basic_server<ssl_tcp_server_connect, channel::publisher>;
	using async_tcp_server = async_server<boost::asio::ip::tcp, async_tcp_session<>>;

	//using no_ssl_tcp_server = basic_server<no_ssl_tcp_server_connect, channel::publisher>;
	using async_ssl_tcp_server = async_server<boost::asio::ip::tcp, async_tcp_ssl_session<>>;

	//using http_server = basic_server<connect<http, conn_mode::basic_server, ssl_mode::nossl>, channel::publisher>;

	//using https_server = basic_server<connect<http, conn_mode::basic_server, ssl_mode::ssl>, channel::publisher>;

	using tcp_client = client<async_tcp_session<>>;

	using no_ssl_tcp_client = client<async_tcp_ssl_session<>>;

	template <typename _Message>
	inline void broadcast(_Message&& msg)
	{
		flex_buffer_t fs{};
		error_code ec{};
		msg.to_binary(fs, ec);

		//session_manager::instance().broadcast(std::move(fs));
	}

	template <typename _Message, typename _Func>
	inline void broadcast(_Message&& msg, _Func&& f)
	{
		flex_buffer_t fs{};
		error_code ec{};
		msg.to_binary(fs, ec);

		//session_manager::instance().broadcast(std::move(fs), std::forward<_Func>(f));
	}
} // namespace aquarius