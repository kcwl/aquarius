#pragma once


namespace aquarius
{
	

#ifdef AQUARIUS_ENABLE_SSL
	template<typename Executor = boost::asio::any_io_executor>
	using async_tcp_ssl_session = basic_session<async_io_service<boost::asio::ip::tcp, Executor, true, ssl>>;
#endif
}