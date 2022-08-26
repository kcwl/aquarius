#pragma once
#include <memory>
#include "router.hpp"
#include "message/message.hpp"
#include "stream.hpp"
#include "connect.hpp"

namespace aquarius
{
	class session : public socket_connect
	{
	public:
		session(boost::asio::io_service& io_service)
			: socket_connect(io_service)
		{

		}

		virtual ~session() = default;

		void start()
		{
			async_read();
		}

		void on_close() 
		{
			return;
		}
	};
}