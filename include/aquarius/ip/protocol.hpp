#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/logger.hpp>

namespace aquarius
{
	template <typename T>
	struct handler_tag_traits;

	struct tcp_protocol
	{
		using socket = boost::asio::ip::tcp::socket;

		using endpoint = boost::asio::ip::tcp::endpoint;

		using acceptor = boost::asio::ip::tcp::acceptor;

		using resolver = boost::asio::ip::tcp::resolver;

		using no_delay = boost::asio::ip::tcp::no_delay;

		using keep_alive = boost::asio::socket_base::keep_alive;
	};

	struct http_protocol
	{
		using socket = boost::asio::ip::tcp::socket;

		using endpoint = boost::asio::ip::tcp::endpoint;

		using acceptor = boost::asio::ip::tcp::acceptor;

		using resolver = boost::asio::ip::tcp::resolver;

		using no_delay = boost::asio::ip::tcp::no_delay;

		using keep_alive = boost::asio::socket_base::keep_alive;
	};

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

		using socket = boost::asio::ip::udp::socket;

		using endpoint = boost::asio::ip::udp::endpoint;

		using acceptor = null_acceptor;

		using resolver = boost::asio::ip::udp::resolver;

		using no_delay = null_delay;

		using keep_alive = boost::asio::socket_base::keep_alive;
	};
} // namespace aquarius