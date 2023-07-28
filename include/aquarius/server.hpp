#pragma once
#include <aquarius/detail/io_service_pool.hpp>
#include <type_traits>
#include <aquarius/defines.hpp>
#include <aquarius/session.hpp>
#include <aquarius/timer_queue.hpp>

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
			timer_queue::instance().exec(io_service_pool_.get_io_service());

			io_service_pool_.run();
		}

		void stop()
		{
			io_service_pool_.stop();
		}

		std::size_t client_count()
		{
			return connect_count_.load(std::memory_order_acq_rel);
		}

		template <connect_event E, typename _Func>
		void regist_callback(_Func&& f)
		{
			if constexpr (E == connect_event::start)
			{
				start_func_ = std::forward<_Func>(f);
			}
			else if constexpr (E == connect_event::close)
			{
				close_func_ = std::forward<_Func>(f);
			}
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
										   new_connect_ptr->regist_callback<connect_event::start>(start_func_);

										   new_connect_ptr->regist_callback<connect_event::close>(close_func_);

										   new_connect_ptr->start();

										   connect_count_.exchange(connect_count_ + 1);
									   }

									   start_accept();
								   });
		}

	private:
		detail::io_service_pool io_service_pool_;

		boost::asio::signal_set signals_;

		boost::asio::ip::tcp::acceptor acceptor_;

		std::atomic_uint32_t connect_count_;

		std::function<void(std::shared_ptr<basic_session>)> start_func_;

		std::function<void(std::shared_ptr<basic_session>)> close_func_;
	};
} // namespace aquarius
