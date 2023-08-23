#pragma once
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <aquarius/client.hpp>
#include <aquarius/context.hpp>
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>
#include <aquarius/server.hpp>
#include <aquarius/connect.hpp>
#include <aquarius/detail/md5.hpp>
#include <aquarius/ip_addr.hpp>

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

	inline std::shared_ptr<basic_session> find_session(std::size_t id)
	{
		return detail::session_manager::instance().find(id);
	}

	inline bool erase_session(std::size_t id)
	{
		return detail::session_manager::instance().erase(id);
	}

	inline std::size_t count_session()
	{
		return detail::session_manager::instance().count();
	}

	template <typename _Message>
	inline void broadcast(_Message&& msg)
	{
		flex_buffer_t fs{};
		msg.visit(fs, visit_mode::output);

		detail::session_manager::instance().broadcast(std::move(fs));
	}

	template <typename _Message, typename _Func>
	inline void broadcast(_Message&& msg, _Func&& f)
	{
		flex_buffer_t fs{};
		msg.visit(fs, visit_mode::output);

		detail::session_manager::instance().broadcast(std::move(fs), std::forward<_Func>(f));
	}

	template <typename _Func>
	std::size_t regist_timer(std::chrono::hours hour, std::chrono::minutes min, std::chrono::seconds sec, _Func&& f)
	{
		auto cur_now = std::chrono::system_clock::now();
		auto hh_cur_now = std::chrono::hh_mm_ss<std::chrono::hours>(cur_now);
		auto dest_time = cur_now;

		dest_time -= hh_cur_now.hours();
		dest_time -= hh_cur_now.minutes();
		dest_time -= hh_cur_now.seconds();

		dest_time += hour;
		dest_time += min;
		dest_time += sec;

		auto dura = dest_time - cur_now;

		dura.count() < 0 ? dura += std::chrono::days(1) : 0;

		return timer_queue::instance().regist(std::forward<_Func>(f), timer_queue::timer_category::clock,
											  std::chrono::duration_cast<std::chrono::milliseconds>(dura));
	}

	template <typename _Func>
	std::size_t regist_timer(std::chrono::milliseconds ms, _Func&& f)
	{
		return timer_queue::instance().regist(std::forward<_Func>(f), timer_queue::timer_category::duration, ms);
	}

} // namespace aquarius