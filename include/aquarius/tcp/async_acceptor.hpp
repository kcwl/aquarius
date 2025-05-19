#pragma once
#include <aquarius/basic_async_acceptor.hpp>
#include <aquarius/detail/asio.hpp>

namespace aquarius
{
	namespace tcp
	{
		using async_acceptor = basic_async_acceptor<boost::asio::ip::tcp, boost::asio::any_io_executor>;
	}
} // namespace aquarius