#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/io_service_pool.hpp>
#include <aquarius/logger.hpp>
#include <type_traits>

namespace aquarius
{
	template <typename _Connector>
	class server
	{
	public:
		explicit server(int32_t port, int io_service_pool_size, const std::string& name = {})
			: io_service_pool_(io_service_pool_size)
			, end_point_(boost::asio::ip::tcp::v4(), static_cast<unsigned short>(port))
			, signals_(io_service_pool_.get_io_service())
			, acceptor_(io_service_pool_.get_io_service(), end_point_)
			, server_name_(name)
		{
			signals_.add(SIGINT);
			signals_.add(SIGTERM);
#ifdef SIGQUIT
			signals_.add(SIGQUIT);
#endif
			signals_.async_wait(std::bind(&server::signal_stop, this, std::placeholders::_1, std::placeholders::_2));

			start_accept();
		}

		virtual ~server() = default;

	public:
		void run()
		{
			init_logger();

			io_service_pool_.run();

			XLOG(info) << "[server] " << server_name_ << " server is started!";
		}

		void stop()
		{
			signal_stop({}, SIGINT);
		}

		std::size_t client_count()
		{
			return connect_count_.load(std::memory_order_acq_rel);
		}

	private:
		void start_accept()
		{
			auto new_connect_ptr = std::make_shared<_Connector>(io_service_pool_.get_io_service());

			acceptor_.async_accept(new_connect_ptr->socket(),
								   [this, new_connect_ptr](const boost::system::error_code& ec)
								   {
									   if (ec)
									   {
										   XLOG(error) << "[acceprtor] accept error at "
													   << end_point_.address().to_string() << ":" << ec.message();

										   close();
									   }
									   else
									   {
										   new_connect_ptr->start();

										   connect_count_.exchange(connect_count_ + 1);

										   start_accept();

										   XLOG(info)
											   << "[acceptor] accept connection at " << end_point_.address().to_string()
											   << " : " << new_connect_ptr->remote_address();
									   }
								   });
		}

		void close()
		{
			boost::system::error_code ec;
			acceptor_.cancel(ec);
			acceptor_.close(ec);

			XLOG(info) << "[acceptor] acceptor closed";
		}

		void signal_stop(const boost::system::error_code& ec, int signal)
		{
			std::string error_message = "success";

			ec ? error_message = ec.message() : std::string{};

			io_service_pool_.stop();

			signals_.cancel();

			close();

			XLOG(info) << "[server] " << server_name_ << " server is stop! result: " << error_message
					   << ", signal: " << signal;
		}

	private:
		io_service_pool io_service_pool_;

		boost::asio::ip::tcp::endpoint end_point_;

		boost::asio::signal_set signals_;

		boost::asio::ip::tcp::acceptor acceptor_;

		std::atomic_uint32_t connect_count_;

		std::string server_name_;
	};
} // namespace aquarius
