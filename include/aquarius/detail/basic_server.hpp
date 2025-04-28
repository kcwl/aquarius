#pragma once
#include <aquarius/core/logger.hpp>
#include <aquarius/detail/io_service_pool.hpp>
#include <boost/asio/signal_set.hpp>

namespace aquarius
{
	template <typename Acceptor, typename Session>
	class basic_server
	{
		using acceptor_type = Acceptor;

		using session_type = Session;

	public:
		explicit basic_server(uint16_t port, int32_t io_service_pool_size, const std::string& name = {})
			: io_service_pool_(io_service_pool_size)
			, signals_(io_service_pool_.get_io_service(), SIGINT, SIGTERM)
			, acceptor_(io_service_pool_.get_io_service(), port)
			, server_name_(name)
		{
			init_signal();

			start_accept();
		}

		~basic_server() = default;

	public:
		void run()
		{
			XLOG_INFO() << "[server] " << server_name_ << " server is started!";

			io_service_pool_.run();
		}

		void stop()
		{
			io_service_pool_.stop();
		}

	private:
		void start_accept()
		{
			boost::system::error_code ec;

			auto sock = acceptor_.accept(ec);

			if (!ec)
			{
				//auto conn_ptr = std::make_shared<session_type>(std::move(sock.await_resume()));

				//conn_ptr->start();
			}

			start_accept();
		}

		void init_signal()
		{
			signals_.async_wait(
				[&](boost::system::error_code ec, int signal)
				{
					std::string error_message = "success";

					ec ? error_message = ec.message() : std::string{};

					acceptor_.close(ec);

					io_service_pool_.stop();

					XLOG_INFO() << "[server] " << server_name_ << " server is stop! result: " << error_message
								<< ", signal: " << signal;
				});
		}

	private:
		io_service_pool io_service_pool_;

		boost::asio::signal_set signals_;

		acceptor_type acceptor_;

		std::string server_name_;
	};
} // namespace aquarius