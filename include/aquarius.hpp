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
	template <std::size_t Identify>
	using no_ssl_tcp_connect = connect<connect_tcp, void, Identify>;

	template <std::size_t Identify>
	using ssl_tcp_connect = connect<connect_tcp, ssl_socket, Identify>;

	template <std::size_t Identify>
	using no_ssl_tcp_server = server<no_ssl_tcp_connect<Identify>>;

	template <std::size_t Identify>
	using ssl_tcp_server = server<ssl_tcp_connect<Identify>>;

	template <std::size_t Identify>
	using no_ssl_http_connect = connect<http, void, Identify>;

	template <std::size_t Identify>
	using ssl_http_connect = connect<http, ssl_socket, Identify>;

	template <std::size_t Identify>
	using no_ssl_http_server = server<no_ssl_http_connect<Identify>>;

	template <std::size_t Identify>
	using ssl_http_server = server<ssl_http_connect<Identify>>;

	template <std::size_t Identify>
	using no_ssl_tcp_client = client<no_ssl_tcp_connect<Identify>>;

	template <std::size_t Identify>
	using ssl_tcp_client = client<ssl_tcp_connect<Identify>>;

	inline std::shared_ptr<xsession> find_session(std::size_t id)
	{
		return session_manager::instance().find(id);
	}

	template <typename _Func>
	inline auto find_session_if(_Func&& f)
	{
		return session_manager::instance().find_if(std::forward<_Func>(f));
	}

	inline bool erase_session(std::size_t id)
	{
		return session_manager::instance().erase(id);
	}

	inline std::size_t count_session()
	{
		return session_manager::instance().count();
	}

	inline void clear_session()
	{
		return session_manager::instance().clear();
	}

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