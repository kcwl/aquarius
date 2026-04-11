#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/logger.hpp>

namespace aquarius
{
	struct udp_protocol
	{
		struct null_acceptor
		{};

		struct null_delay
		{
			null_delay(bool)
			{
				XLOG_ERROR() << "udp not support delay algrothm and operator is not effective";
			}
		};

		using socket = asio::ip::udp::socket;

		using endpoint = asio::ip::udp::endpoint;

		using acceptor = null_acceptor;

		using resolver = asio::ip::udp::resolver;

		using no_delay = null_delay;

		using keep_alive = asio::socket_base::keep_alive;
	};
} // namespace aquarius