#pragma once
#include <aquarius/basic_async_io_service.hpp>
#include <aquarius/detail/asio.hpp>

namespace aquarius
{
	namespace tcp
	{
		template <template<typename,typename>typename SSLService, typename Executor = boost::asio::any_io_executor>
		using async_io_service = basic_async_io_service<boost::asio::ip::tcp, Executor, SSLService<boost::asio::ip::tcp, Executor>>;
	}
} // namespace aquarius