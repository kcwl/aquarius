#pragma once
#include <aquarius/core/basic_module.hpp>
#include <aquarius/server/io_service_pool.hpp>

namespace aquarius
{
	namespace srv
	{
		template <typename _Connector>
		class server
		{
		public:
			explicit server(const std::string& port, int io_service_pool_size)
				: io_service_pool_(io_service_pool_size)
				, signals_(io_service_pool_.get_io_service())
				, acceptor_(io_service_pool_.get_io_service(),
							boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
														   static_cast<unsigned short>(std::atoi(port.data()))))
			{
				signals_.add(SIGINT);
				signals_.add(SIGTERM);
#ifdef SIGQUIT
				signals_.add(SIGQUIT);
#endif
				signals_.async_wait(std::bind(&server::handle_stop, this));

				start_accept();
			}

			virtual ~server() = default;

		public:
			bool run()
			{
				if (!load_module())
					return false;

				if (!start_module())
					return false;

				io_service_pool_.run();
			}

		protected:
			virtual bool load_module()
			{
				return true;
			}

			void regist_module(std::shared_ptr<core::basic_module> module_ptr)
			{
				modules_.push_back(module_ptr);
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

			bool start_module()
			{
				for (auto& module : modules_)
				{
					if (!module.enable())
						continue;

					if (!module.initialize())
						return false;

					if (!module.stop())
					{
						return false;
					}
				}

				return true;
			}

			bool stop_module()
			{
				for (auto& module : modules_)
				{
					if (!module.enable())
						continue;

					if (!module.initialize())
						return false;

					if (!module.stop())
					{
						return false;
					}
				}

				return true;
			}

			void handle_stop()
			{
				stop_module();

				io_service_pool_.stop();
			}

		private:
			io_service_pool io_service_pool_;

			boost::asio::signal_set signals_;

			boost::asio::ip::tcp::acceptor acceptor_;

			std::vector<std::shared_ptr<core::basic_module>> modules_;
		};
	} // namespace srv
} // namespace aquarius
