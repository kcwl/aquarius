#pragma once
#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>

namespace aquarius
{
	namespace impl
	{
		struct default_socket
		{};
		struct ssl_socket
		{};

		template <typename _SocketType, typename _SocketBase = boost::asio::ip::tcp::socket>
		class multi_socket
		{
		public:
			template <typename _Execution>
			multi_socket(_Execution& executor,
						 boost::asio::ssl::context ctx = boost::asio::ssl::context(boost::asio::ssl::context::sslv23))
				: socket_(executor)
				, ssl_context_(ctx)
			{
				init();
			}

		public:
			auto& socket()
			{
				return socket_->lowest_layer();
			}

		private:
			void init()
			{
				if constexpr (std::same_as<_SocketType, ssl_socket>)
				{
					boost::system::error_code ec;

					std::string certificate_chain_file;
					std::string private_key_file;

					ssl_context_.set_options(boost::asio::ssl::context::no_sslv3, ec);
					ssl_context_.use_certificate_chain_file(certificate_chain_file, ec);
					ssl_context_.use_private_key_file(private_key_file, boost::asio::ssl::context::pem, ec);
				}
			}

		private:
			boost::asio::ssl::stream<_SocketBase> socket_;

			boost::asio::ssl::context ssl_context_;
		};

	} // namespace impl

	using nossl_socket = impl::multi_socket<void>;
	using ssl_socket = impl::multi_socket<impl::ssl_socket>;
} // namespace aquarius