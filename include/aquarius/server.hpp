#pragma once
#pragma warning(disable:4996)
#include <boost/asio.hpp>
#include "io_service_pool.hpp"
#include "connect.hpp"

namespace aquarius
{
	namespace aqnet
	{
		class server
		{
		public:
			explicit server(const std::string& port, int io_service_pool_size)
				: io_service_pool_(io_service_pool_size)
				, signals_(io_service_pool_.get_io_service())
				, acceptor_(io_service_pool_.get_io_service(), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::atoi(port.c_str())))
#ifdef _SSL_SERVER
				, _context(boost::asio::ssl::context::sslv23)
				{
					init_ssl_param(std::bind(&aq_server::get_passwd, this), "", "", "");
#else
				{
#endif
					signals_.add(SIGINT);
					signals_.add(SIGTERM);
#ifdef SIGQUIT
					signals_.add(SIGQUIT);
#endif
					signals_.async_wait(std::bind(&server::handle_stop, this));

					start_accept();
				}

			virtual~server() = default;

		public:
			void run()
			{
				io_service_pool_.run();
			}

		private:
			void start_accept()
			{
				auto new_connect_ptr = std::make_shared<connect>(io_service_pool_.get_io_service());

				acceptor_.async_accept(new_connect_ptr->socket(),
					[this, new_connect_ptr](const boost::system::error_code& error)
					{
						if(!error)
						{
							new_connect_ptr->start();

							start_accept();
						}
					}
				);
			}

			void handle_stop()
			{
				io_service_pool_.stop();
			}

#ifdef _SSL_SERVER
			template<class Func>
			void init_ssl_param(Func && f, std::string certificate_chain_file, std::string private_key_file, std::string tmp_dh_file)
			{
				_context.set_password_callback(std::forward<Func>(f));

				_context.use_certificate_chain_file(certificate_chain_file);
				_context.use_private_key_file(private_key_file, boost::asio::ssl::context::pem);
				_context.use_tmp_dh_file(tmp_dh_file);
		}
#endif

		private:
			io_service_pool io_service_pool_;

			boost::asio::signal_set signals_;

			tcp::acceptor acceptor_;

			int size_;

#ifdef _SSL_SERVER
			boost::asio::ssl::context _context;
#endif
			std::shared_ptr<connect> new_connection_ptr_;
	};
}
}