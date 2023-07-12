#pragma once
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <aquarius/client.hpp>
#include <aquarius/context.hpp>
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>
#include <aquarius/server.hpp>
#include <aquarius/connect.hpp>

namespace aquarius
{
	using no_ssl_tcp_connect = connect<connect_tcp>;

	using ssl_tcp_connect = connect<connect_tcp, ssl_socket>;

	using no_ssl_tcp_server = server<no_ssl_tcp_connect>;

	using ssl_tcp_server = server<ssl_tcp_connect>;

	using no_ssl_http_connect = connect<connect_http>;

	using ssl_http_connect = connect<connect_http, ssl_socket>;

	using no_ssl_http_server = server<no_ssl_http_connect>;

	using ssl_http_server = server<ssl_http_connect>;

	using no_ssl_tcp_client = client<no_ssl_tcp_connect>;

	using ssl_tcp_client = client<ssl_tcp_connect>;

	std::shared_ptr<basic_session> find_session(std::size_t id)
	{
		return detail::session_manager::instance().find(id);
	}

	bool erase_session(std::size_t id)
	{
		return detail::session_manager::instance().erase(id);
	}

	template<typename _Message>
	void broadcast(_Message&& msg)
	{
		flex_buffer_t fs{};
		msg.visit(fs, visit_mode::output);

		detail::session_manager::instance().broadcast(std::move(fs));
	}

	template<typename _Message, typename _Func>
	void broadcast(_Message&& msg, _Func&& f)
	{
		flex_buffer_t fs{};
		msg.visit(fs, visit_mode::output);

		detail::session_manager::instance().broadcast(std::move(fs), std::forward<_Func>(f));
	}

} // namespace aquarius