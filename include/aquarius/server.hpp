#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/invoke.hpp>
#include <aquarius/io_service_pool.hpp>
#include <aquarius/logger.hpp>
#include <type_traits>

namespace aquarius
{
	template <typename _Connector>
	class server
	{
		using ssl_context_t = asio::ssl::context;

		using connect_t = _Connector;

	public:
		explicit server(int32_t port, int io_service_pool_size, const std::string& name = {})
			: io_service_pool_(io_service_pool_size)
			, endpoint_(asio::ip::tcp::v4(), static_cast<asio::ip::port_type>(port))
			, signals_(io_service_pool_.get_io_service())
			, acceptor_(io_service_pool_.get_io_service(), endpoint_)
			, ssl_context_(asio::ssl::context::sslv23)
			, server_name_(name)
		{
			init_ssl_context();

			init_signal();

			start_accept();
		}

		~server() = default;

	public:
		void run()
		{
			init_logger();

			XLOG(info) << "[server] " << server_name_ << " server is started!";

			service_invoke_helper::run();

			io_service_pool_.run();
		}

		void stop()
		{
			signal_stop({}, -1);
		}

		void close()
		{
			if (!acceptor_.is_open())
				return;

			boost::system::error_code ec;
			acceptor_.cancel(ec);
			acceptor_.close(ec);

			XLOG(info) << "[acceptor] acceptor closed";
		}

	private:
		void start_accept()
		{
			acceptor_.async_accept(io_service_pool_.get_io_service(),
								   [this](const boost::system::error_code& ec, asio::ip::tcp::socket sock)
								   {
									   if (!acceptor_.is_open())
									   {
										   XLOG(error) << "[acceprtor] accept error at "
													   << endpoint_.address().to_string() << ": acceptor is closed!";

										   return;
									   }

									   if (!ec)
									   {
										   std::string ip_addrs{};

										   auto conn_ptr = std::make_shared<connect_t>(std::move(sock), ssl_context_);

										   if (!conn_ptr)
											   return;

										   conn_ptr->start();

										   XLOG(info)
											   << "[acceptor] accept connection at " << endpoint_.address().to_string()
											   << " : " << conn_ptr->remote_address();
									   }
									   else
									   {
										   XLOG(error)
											   << "[acceprtor] occur error at " << endpoint_.address().to_string()
											   << ":" << endpoint_.port() << '\t' << ec.message();
									   }

									   start_accept();
								   });
		}

		void signal_stop(const boost::system::error_code& ec, int signal)
		{
			std::string error_message = "success";

			ec ? error_message = ec.message() : std::string{};

			io_service_pool_.stop();

			close();

			service_invoke_helper::stop();

			XLOG(info) << "[server] " << server_name_ << " server is stop! result: " << error_message
					   << ", signal: " << signal;
		}

		void init_ssl_context()
		{
			ssl_context_.set_options(ssl_context_t::default_workarounds | ssl_context_t::no_sslv2 |
									 ssl_context_t::single_dh_use);

			ssl_context_.set_password_callback(std::bind(&server::get_passwd, this));
			ssl_context_.use_certificate_chain_file("crt/server.crt");
			ssl_context_.use_private_key_file("crt/server.key", ssl_context_t::pem);
			ssl_context_.use_tmp_dh_file("crt/dh512.pem");
		}

		std::string get_passwd()
		{
			return {};
		}

		void init_signal()
		{
			signals_.add(SIGINT);
			signals_.add(SIGTERM);
#ifdef SIGQUIT
			signals_.add(SIGQUIT);
#endif

			signals_.async_wait(std::bind(&server::signal_stop, this, std::placeholders::_1, std::placeholders::_2));
		}

	private:
		io_service_pool io_service_pool_;

		asio::ip::tcp::endpoint endpoint_;

		asio::signal_set signals_;

		asio::ip::tcp::acceptor acceptor_;

		ssl_context_t ssl_context_;

		std::string server_name_;
	};
} // namespace aquarius
