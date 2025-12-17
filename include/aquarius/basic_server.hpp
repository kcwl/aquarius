#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/detail/make_endpoint.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/io_service_pool.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/module/module_router.hpp>
#include <aquarius/session_store.hpp>
#include <aquarius/timer.hpp>

using namespace std::chrono_literals;

namespace aquarius
{
	template <typename Session>
	class basic_server
	{
		using session_type = Session;

		using acceptor = typename Session::acceptor;

		using endpoint = typename acceptor::endpoint_type;

	public:
		explicit basic_server(uint16_t port, int32_t io_service_pool_size, std::chrono::milliseconds global_time_dura,
							  const std::string& name = {})
			: io_service_pool_size_(io_service_pool_size)
			, io_service_pool_(io_service_pool_size_)
			, signals_(io_service_pool_.get_io_service(), SIGINT, SIGTERM)
			, acceptor_(io_service_pool_.get_io_service(), detail::make_v4_endpoint(port))
			, server_name_(name)
			, global_timer_(io_service_pool_.get_io_service(), global_time_dura)
		{
			init_signal();

			init_global_timer();

			co_spawn(acceptor_.get_executor(), start_accept(), detached);
		}

		~basic_server() = default;

	public:
		void run()
		{
			XLOG_INFO() << "[server] " << server_name_ << " server is started!";

			module_router::get_mutable_instance().run();

			io_service_pool_.run();
		}

		void stop()
		{
			acceptor_.close();

			io_service_pool_.stop();
		}

		bool enable() const
		{
			return acceptor_.is_open() && io_service_pool_.enable();
		}

	private:
		auto start_accept() -> awaitable<void>
		{
			error_code ec;

			for (;;)
			{
				auto sock = co_await acceptor_.async_accept(redirect_error(use_awaitable, ec));

				if (!acceptor_.is_open())
				{
					ec = error::bad_descriptor;
				}

				if (ec)
					break;

				auto session_ptr = std::make_shared<session_type>(std::move(sock), 1s);

				regist_session(session_ptr);

				co_spawn(
					acceptor_.get_executor(),
					[session_ptr] -> awaitable<void> { co_return co_await session_ptr->accept(); }, detached);
			}
		}

		void init_signal()
		{
			signals_.async_wait(
				[&](error_code ec, int signal)
				{
					std::string error_message = "success";

					ec ? error_message = ec.message() : std::string{};

					acceptor_.close();

					io_service_pool_.stop();

					XLOG_INFO() << "[server] " << server_name_ << " server is stop! result: " << error_message
								<< ", signal: " << signal;
				});
		}

		void init_global_timer()
		{
			timer_.async_wait(
				[this, self = this->shared_from_this()](error_code ec)
				{
					if (ec)
					{
						XLOG_ERROR() << "globale timer error: " << ec.what();
						return;
					}

					module_router::get_mutable_instance().timer(timer_.dura());
				});
		}

	protected:
		std::size_t io_service_pool_size_;

		io_service_pool io_service_pool_;

		signal_set signals_;

		acceptor acceptor_;

		std::string server_name_;

		timer<steady_timer> global_timer_;
	};
} // namespace aquarius