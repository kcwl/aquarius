#pragma once
#include <aquarius/client.hpp>
#include <aquarius/defines.hpp>
#include <aquarius/io_service_pool.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/service.hpp>
#include <type_traits>

namespace aquarius
{
	template <typename _Connector>
	class server
	{
	public:
		constexpr static std::size_t IDENTIFY = _Connector::IDENTIFY;

	public:
		explicit server(int32_t port, int io_service_pool_size, const std::string& name = {})
			: io_service_pool_(io_service_pool_size)
			, endpoint_(boost::asio::ip::tcp::v4(), static_cast<unsigned short>(port))
			, signals_(io_service_pool_.get_io_service())
			, acceptor_(io_service_pool_.get_io_service(), endpoint_)
			, server_name_(name)
			, is_master_(false)
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

			XLOG(info) << "[server] " << server_name_ << " server is started!";

			service_router::instance().run();

			io_service_pool_.run();
		}

		void stop()
		{
			signal_stop({}, SIGINT);
		}

		void set_master(bool master = true)
		{
			is_master_ = master;
		}

		void set_master_address(const std::string& ip_addr, int32_t port)
		{
			if (is_master_)
				return;

			slave_client_ptr_.reset(
				new client<_Connector>(io_service_pool_.get_io_service(), ip_addr, std::to_string(port)));

			slave_client_ptr_->set_report();

			slave_client_ptr_->set_server_port(endpoint_.port());
		}

	private:
		void start_accept()
		{
			auto new_connect_ptr = std::make_shared<_Connector>(io_service_pool_.get_io_service());

			acceptor_.async_accept(
				new_connect_ptr->socket(),
				[this, new_connect_ptr](const boost::system::error_code& ec)
				{
					if (ec)
					{
						XLOG(error) << "[acceprtor] accept error at " << endpoint_.address().to_string() << ":"
									<< ec.message();

						close();
					}
					else
					{
						std::string ip_addrs{};

						if (slave_session(new_connect_ptr->remote_address(), ip_addrs))
						{
							auto [_ip_addr, _port] = spilt_ipaddr(ip_addrs);

							
							flex_buffer_t buffer{};

							buffer.sputn((uint8_t*)&ip_back_proto, sizeof(ip_back_proto));
							response_header header{};
							header.result_ = boost::asio::ip::address_v4().from_string(_ip_addr).to_uint();
							header.reserve_ = _port;

							buffer.sputn((uint8_t*)&header, sizeof(header));

							new_connect_ptr->async_write(std::move(buffer), [] {});

							XLOG(info) << "[acceptor] transfer connection at " << endpoint_.address().to_string()
									   << " : " << new_connect_ptr->remote_address() << " to " << _ip_addr << ":"
									   << _port << "\n";
						}
						else
						{
							new_connect_ptr->start();

							XLOG(info) << "[acceptor] accept connection at " << endpoint_.address().to_string()
									   << " : " << new_connect_ptr->remote_address();
						}

						start_accept();
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

			service_router::instance().stop();

			XLOG(info) << "[server] " << server_name_ << " server is stop! result: " << error_message
					   << ", signal: " << signal;
		}

		auto spilt_ipaddr(const std::string& addr) -> std::pair<std::string, int32_t>
		{
			auto pos = addr.find_first_of(":");

			if (pos == std::string::npos)
				return { {}, {} };

			return { addr.substr(0, pos), std::atoi(addr.substr(pos + 1).c_str()) };
		}

	private:
		io_service_pool io_service_pool_;

		boost::asio::ip::tcp::endpoint endpoint_;

		boost::asio::signal_set signals_;

		boost::asio::ip::tcp::acceptor acceptor_;

		std::string server_name_;

		bool is_master_;

		std::shared_ptr<client<_Connector>> slave_client_ptr_;
	};
} // namespace aquarius
