#pragma once
#include "../core/io_service_pool.hpp"
#include <aquarius/session.hpp>

namespace aquarius
{
	namespace srv
	{
		class server
		{
		public:
			explicit server(const std::string& port, int io_service_pool_size)
				: io_service_pool_(io_service_pool_size)
				, signals_(io_service_pool_.get_io_service())
				, acceptor_(io_service_pool_.get_io_service(), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), static_cast<unsigned short>(std::atoi(port.data()))))
			{
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
			virtual void start_accept()
			{
				acceptor_.set_option(boost::asio::socket_base::reuse_address());

				auto new_connect_ptr = std::make_shared<socket_connect>(io_service_pool_.get_io_service());

				acceptor_.async_accept(new_connect_ptr->socket(),
					[this, new_connect_ptr](const boost::system::error_code& error)
					{
						if (!error)
						{
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

		private:
			core::io_service_pool io_service_pool_;

			boost::asio::signal_set signals_;

			boost::asio::ip::tcp::acceptor acceptor_;
		};
	}
}