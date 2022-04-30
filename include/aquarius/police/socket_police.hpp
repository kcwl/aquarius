#pragma once
#include <boost/asio.hpp>

#ifdef ENABLE_SSL
#include <boost/asio/ssl.hpp>
#endif

namespace aquarius
{
	struct normal_socket_establish_police
	{
		using socket_t = boost::asio::ip::tcp::socket;

		template<typename T>
		static void async_shake(std::shared_ptr<T> conn_ptr)
		{
			conn_ptr->establish_async_read();
		}

		template<typename T>
		static auto& socket(std::shared_ptr<T> conn_ptr)
		{
			return conn_ptr->socket();
		}
	};

	struct normal_socket_police
	{
		using socket_t = boost::asio::ip::tcp::socket;

		template<typename T>
		static void async_shake(std::shared_ptr<T> conn_ptr)
		{
			conn_ptr->async_read();
		}

		template<typename T>
		static auto& socket(std::shared_ptr<T> conn_ptr)
		{
			return conn_ptr->socket();
		}
	};

#ifdef ENABLE_SSL
	struct ssl_socket_establish_police
	{
		using socket_t = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

		template<typename T>
		static void async_shake(std::shared_ptr<T> conn_ptr)
		{
			conn_ptr->socket().async_handshake(boost::asio::ssl::stream_base::server,
				[conn_ptr](const boost::system::error_code& error)
				{
					if (error)
					{
						std::cout << error.message() << std::endl;
						return;
					}

					conn_ptr->establish_async_read();
				});
		}

		template<typename T>
		static auto& socket(std::shared_ptr<T> conn_ptr)
		{
			return conn_ptr->socket_.lowest_layer();
		}
	};

	struct ssl_socket_police
	{
		using socket_t = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

		template<typename T>
		static void async_shake(std::shared_ptr<T> conn_ptr)
		{
			conn_ptr->socket().async_handshake(boost::asio::ssl::stream_base::server,
				[conn_ptr](const boost::system::error_code& error)
				{
					if (error)
					{
						std::cout << error.message() << std::endl;
						return;
					}

					conn_ptr->async_read();
				});
		}

		template<typename T>
		static auto& socket(std::shared_ptr<T> conn_ptr)
		{
			return conn_ptr->socket_.lowest_layer();
		}
	};
#endif
}