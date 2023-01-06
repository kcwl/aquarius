#pragma once
#include <aquarius/impl/ssl_context.hpp>
#include <boost/asio.hpp>

namespace aquarius
{
	namespace impl
	{
		struct default_socket
		{};
		struct ssl_socket
		{};

		template <typename _Sock_Type = void>
		class socket : public boost::asio::ip::tcp::socket
		{
		public:
			template <typename _Execution_Context>
			socket(_Execution_Context& executor)
				: boost::asio::ip::tcp::socket(executor)
			{}

		public:
			auto& sock()
			{
				return *this;
			}
		};

#if ENABLE_SSL
		template <>
		class socket<ssl_socket> : public boost::asio::ssl::stream<boost::asio::ip::tcp::socket>
		{
			using executor_type = boost::asio::ip::tcp::socket::executor_type;

		public:
			socket(const executor_type& executor)
				: boost::asio::ip::tcp::socket(executor)
			{}

		public:
			auto& sock()
			{
				return this->lowest_layer();
			}
		};
#endif
	} // namespace sock
} // namespace aquarius