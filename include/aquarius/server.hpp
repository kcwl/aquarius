#pragma once
#pragma warning(disable:4996)
#include <boost/asio.hpp>
#include "io_service_pool.hpp"
#include "connect.hpp"

namespace aquarius
{
	class server
		: public detail::callback<connect>
	{
	public:
		explicit server(const std::string& port, int io_service_pool_size)
			: io_service_pool_(io_service_pool_size)
			, signals_(io_service_pool_.get_io_service())
			, acceptor_(io_service_pool_.get_io_service(), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), static_cast<unsigned short>(std::atoi(port.data()))))
#ifdef _SSL_SERVER
			, ssl_context_(boost::asio::ssl::context::sslv23)
		{
			init_ssl_param(std::bind(&server::get_passwd, this), "", "", "");
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

		void set_connect_cb(connect_callback cb)
		{
			conn_cb_ = cb;
		}

		void set_disconnect_cb(disconnect_callback cb)
		{
			disconn_cb_ = cb;
		}

	private:
		void start_accept()
		{
			auto new_connect_ptr = std::make_shared<connect>(io_service_pool_.get_io_service());

			acceptor_.async_accept(new_connect_ptr->socket(),
				[this, new_connect_ptr](const boost::system::error_code& error)
				{
					if (!error)
					{
						new_connect_ptr->set_connect_cb(conn_cb_);
						
						new_connect_ptr->set_disconnect_cb(disconn_cb_);

						new_connect_ptr->start();
					}

					start_accept();
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
			ssl_context_.set_password_callback(std::forward<Func>(f));

			ssl_context_.use_certificate_chain_file(certificate_chain_file);
			ssl_context_.use_private_key_file(private_key_file, boost::asio::ssl::context::pem);
			ssl_context_.use_tmp_dh_file(tmp_dh_file);
		}

		void get_passwd() {}
#endif

	private:
		io_service_pool io_service_pool_;

		boost::asio::signal_set signals_;

		tcp::acceptor acceptor_;

		inline static int number_ = 0;

#ifdef _SSL_SERVER
		boost::asio::ssl::context ssl_context_;
#endif
	};
}