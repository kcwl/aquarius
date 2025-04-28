#pragma once
#include <aquarius/server/socket_io_base.hpp>

namespace aquarius
{
	template<typename Protocol, typename Executor>
	class ssl_socket_io_base : public socket_io_base<Protocol, Executor>
	{
	public:

	};
}