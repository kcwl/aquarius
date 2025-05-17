#pragma once
#include <aquarius/basic_session_service.hpp>
#include <aquarius/detail/asio.hpp>

namespace aquarius
{
	namespace tcp
	{
		template <typename Executor = boost::asio::any_io_executor>
		using session_service = basic_session_service<boost::asio::ip::tcp, Executor>;
	}
} // namespace aquarius