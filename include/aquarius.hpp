#pragma once
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <aquarius/client.hpp>
#include <aquarius/connect.hpp>
#include <aquarius/context.hpp>
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>
#include <aquarius/server.hpp>
#include <aquarius/invoke.hpp>

namespace aquarius
{
	template <std::size_t Identify>
	using tcp_server_connect = server_connect<tcp, Identify>;

	template <std::size_t Identify>
	using tcp_server = server<tcp_server_connect<Identify>>;

	template <std::size_t Identify>
	using http_server_connect = server_connect<http, Identify>;

	template <std::size_t Identify>
	using http_server = server<http_server_connect<Identify>>;

	using tcp_client_connect = client_connect<tcp>;

	using tcp_client = client<tcp_client_connect>;

	template <typename _Message>
	inline void broadcast(_Message&& msg)
	{
		flex_buffer_t fs{};
		msg.to_binary(fs);

		session_manager::instance().broadcast(std::move(fs));
	}

	template <typename _Message, typename _Func>
	inline void broadcast(_Message&& msg, _Func&& f)
	{
		flex_buffer_t fs{};
		msg.to_binary(fs);

		session_manager::instance().broadcast(std::move(fs), std::forward<_Func>(f));
	}
} // namespace aquarius