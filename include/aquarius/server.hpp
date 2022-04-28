#pragma once
#pragma warning(disable:4996)
#include "connect.hpp"
#include "io_service_pool.hpp"

namespace aquarius
{
	template<typename CallbackPolice>
	class basic_server
	{
	public:
		explicit basic_server(const std::string& port, int io_service_pool_size)
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
				signals_.async_wait(std::bind(&basic_server::handle_stop, this));

				start_accept();
			}

		virtual~basic_server() = default;

	public:
		void run()
		{
			io_service_pool_.run();
		}

		template<typename F>
		void set_connect_cb(F&& cb)
		{
			cb_ptr_.regist_callback(CallbackPolice::police_type::conn, std::forward<F>(cb));
		}

		template<typename F>
		void set_disconnect_cb(F&& cb)
		{
			cb_ptr_.regist_callback(CallbackPolice::police_type::disconn, std::forward<F>(cb));
		}

	private:
		void start_accept()
		{

			auto new_connect_ptr = std::make_shared<connect>(io_service_pool_.get_io_service()
#ifdef ENABLE_SSL
				, context_
#endif
				);

			acceptor_.async_accept(new_connect_ptr->socket(),
				[this, new_connect_ptr](const boost::system::error_code& error)
				{
					if (!error)
					{
						new_connect_ptr->set_callback(cb_ptr_);

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
		io_service_pool io_service_pool_;

		boost::asio::signal_set signals_;

		boost::asio::ip::tcp::acceptor acceptor_;

		inline static int number_ = 0;

		std::shared_ptr<CallbackPolice> cb_ptr_;

#ifdef ENABLE_SSL
		boost::asio::ssl::context context_;
#endif
	};

	using server = basic_server<callback_police>;
}