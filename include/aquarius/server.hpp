#pragma once
#include <aquarius/detail/io_service_pool.hpp>
#include <type_traits>

namespace aquarius
{
	template <typename _Connector>
	class server
	{
	public:
		explicit server(int32_t port, int io_service_pool_size)
			: io_service_pool_(io_service_pool_size)
			, signals_(io_service_pool_.get_io_service())
			, acceptor_(io_service_pool_.get_io_service(),
						boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
													   static_cast<unsigned short>(port)))
		{
			signals_.add(SIGINT);
			signals_.add(SIGTERM);
#ifdef SIGQUIT
			signals_.add(SIGQUIT);
#endif
			signals_.async_wait(std::bind(&server::stop, this));

			start_accept();
		}

		virtual ~server() = default;

	public:
		void run()
		{
			io_service_pool_.run();
		}

		void stop()
		{
			io_service_pool_.stop();
		}

	private:
		void start_accept()
		{
			auto new_connect_ptr = std::make_shared<_Connector>(io_service_pool_.get_io_service());

			acceptor_.async_accept(new_connect_ptr->socket(),
								   [this, new_connect_ptr](const boost::system::error_code& error)
								   {
									   if (!error)
									   {
										   new_connect_ptr->start();
									   }

									   start_accept();
								   });
		}

	private:
		detail::io_service_pool io_service_pool_;

		boost::asio::signal_set signals_;

		boost::asio::ip::tcp::acceptor acceptor_;
	};
} // namespace aquarius
