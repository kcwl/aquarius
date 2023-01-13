#pragma once
#include <aquarius/impl/defines.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace aquarius
{
	namespace impl
	{
		template <typename _SocketType, typename _SocketBase = boost::asio::ip::tcp::socket>
		class multi_socket
		{
		public:
			template <typename _Execution>
			multi_socket(_Execution& executor)
				: socket_(executor)
				, ssl_context_(boost::asio::ssl::context::sslv23)
				, ssl_socket_(std::move(socket_), ssl_context_)
			{
				init();
			}

		public:
			auto& socket()
			{
				if constexpr (std::same_as<_SocketType, ssl_socket>)
				{
					return ssl_socket_.lowest_layer();
				}
				else
				{
					return socket_;
				}
			}

			template <typename _Func>
			void async_handshake(_Func&& f)
			{
				if constexpr (std::same_as<_SocketType, ssl_socket>)
				{
					ssl_socket_.async_handshake(boost::asio::ssl::stream_base::server,
												[this](const boost::system::error_code& ec)
												{
													std::forward<_Func>(f)(ec);
												});
				}
				else
				{
					f({});
				}
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
			_SocketBase socket_;

			boost::asio::ssl::context ssl_context_;

			boost::asio::ssl::stream<_SocketBase> ssl_socket_;
		};

	} // namespace impl

	using nossl_socket_t = impl::multi_socket<void>;
	using ssl_socket_t = impl::multi_socket<impl::ssl_socket>;
} // namespace aquarius