#pragma once
#include <aquarius/basic_async_io_service.hpp>
#include <aquarius/detail/asio.hpp>

namespace aquarius
{
	namespace tcp
	{
		template <typename SSLService, typename Executor = any_io_executor>
		using async_server_io_service = basic_async_io_service<ip::tcp, Executor, SSLService>;
	}
} // namespace aquarius