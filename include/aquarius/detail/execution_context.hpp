#pragma once
#include <boost/asio/execution_context.hpp>

namespace aquarius
{
	namespace detail
	{
		using boost::asio::detail::execution_context_service_base;

		using boost::asio::execution_context;
	}
}