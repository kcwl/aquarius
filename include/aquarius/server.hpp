#pragma once
#include <aquarius/server/io_service_pool.hpp>

namespace aquarius
{
	template <typename _Connector, typename _Pattern>
	class basic_server : public _Pattern
	{
		using connect_t = _Connector;

	public:
		explicit basic_server(int32_t port, int io_service_pool_size, const std::string& name = {})
			: io_service_pool_(io_service_pool_size)
			, endpoint_(asio::ip::tcp::v4(), static_cast<uint16_t>(port))
			, signals_(io_service_pool_.get_io_service())
			, acceptor_(io_service_pool_.get_io_service(), endpoint_)
			, ssl_context_(asio::ssl_context_t::sslv23)
			, server_name_(name)
			, on_accept_()
			, on_close_()
		{
			init_ssl_context();

			init_signal();

			start_accept();
		}

		~basic_server() = default;

	public:
		void run()
		{
			init_logger();

			XLOG_INFO() << "[server] " << server_name_ << " server is started!";

			this->publish("service", "run");

			io_service_pool_.run();
		}

		void stop()
		{
			signal_stop({}, -1);
		}

		void close()
		{
			asio::error_code ec;
			acceptor_.cancel(ec);
			acceptor_.close(ec);

			XLOG_INFO() << "[acceptor] acceptor closed";
		}

		template<typename _Func>
		void regist_on_accept(_Func&& f)
		{
			on_accept_ = std::forward<_Func>(f);
		}

		template<typename _Func>
		void regist_on_close(_Func&& f)
		{
			on_close_ = std::forward<_Func>(f);
		}

	private:
		void start_accept()
		{
			acceptor_.async_accept(io_service_pool_.get_io_service(),
								   [this](const asio::error_code& ec, asio::ip::tcp::socket sock)
								   {
									   if (!acceptor_.is_open())
									   {
										   XLOG_ERROR() << "[acceprtor] accept error at "
														<< endpoint_.address().to_string() << ": acceptor is closed!";

										   return;
									   }

									   if (!ec)
									   {
										   auto conn_ptr = std::make_shared<connect_t>(std::move(sock), ssl_context_);

										   conn_ptr->regist_accept(on_accept_);

										   conn_ptr->regist_close(on_close_);

										   conn_ptr->start();

										   XLOG_INFO()
											   << "[acceptor] accept connection at " << endpoint_.address().to_string()
											   << " : " << conn_ptr->remote_address() << "("
											   << conn_ptr->remote_address_u() << ")[" << conn_ptr->remote_port()
											   << "]";
									   }

									   start_accept();
								   });
		}

		void signal_stop(asio::error_code ec, int signal)
		{
			std::string error_message = "success";

			ec ? error_message = ec.message() : std::string{};

			close();

			this->publish("service", "stop");

			io_service_pool_.stop();

			XLOG_INFO() << "[server] " << server_name_ << " server is stop! result: " << error_message
						<< ", signal: " << signal;
		}

		void init_ssl_context()
		{
			ssl_context_.set_options(asio::ssl_context_t::default_workarounds | asio::ssl_context_t::no_sslv2 |
									 asio::ssl_context_t::single_dh_use);

			ssl_context_.use_certificate_chain_file("crt/server.crt");
			ssl_context_.use_private_key_file("crt/server.key", asio::ssl_context_t::pem);
			ssl_context_.use_tmp_dh_file("crt/dh512.pem");
		}

		void init_signal()
		{
			signals_.add(SIGINT);
			signals_.add(SIGTERM);
#ifdef SIGQUIT
			signals_.add(SIGQUIT);
#endif

			signals_.async_wait(
				std::bind(&basic_server::signal_stop, this, std::placeholders::_1, std::placeholders::_2));
		}

	private:
		io_service_pool io_service_pool_;

		asio::ip::tcp::endpoint endpoint_;

		asio::signal_set signals_;

		asio::ip::tcp::acceptor acceptor_;

		asio::ssl_context_t ssl_context_;

		std::string server_name_;

		std::function<void(const std::size_t)> on_accept_;

		std::function<void(const std::size_t)> on_close_;
	};
} // namespace aquarius
