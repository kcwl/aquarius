#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/co_spawn.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/io_service_pool.hpp>
#include <aquarius/detail/signal_set.hpp>
#include <aquarius/detail/session_store.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/use_awaitable.hpp>
#include <aquarius/detail/visit.hpp>
#include <aquarius/detail/make_endpoint.hpp>

namespace aquarius
{
	template <typename Session, typename... Adaptor>
	class basic_server
	{
		using session_type = Session;

		using acceptor = typename Session::acceptor;

		using endpoint = typename acceptor::endpoint_type;

	public:
		explicit basic_server(uint16_t port, int32_t io_service_pool_size, const std::string& name = {})
			: io_service_pool_(io_service_pool_size)
			, signals_(io_service_pool_.get_io_service(), SIGINT, SIGTERM)
			, acceptor_(io_service_pool_.get_io_service(), detail::make_v4_endpoint(port))
			, server_name_(name)
			, adaptors_()
		{
			init_signal();

			co_spawn(acceptor_.get_executor(), start_accept(), detached);
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
			acceptor_.close();

			io_service_pool_.stop();
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
					ec = boost::asio::error::bad_descriptor;
				}

				if (ec)
					break;

				auto session_ptr = std::make_shared<session_type>(std::move(sock));

				detail::store<session_type>(session_ptr);

				co_spawn(
					acceptor_.get_executor(), [session_ptr] -> awaitable<error_code>
					{
						co_return co_await session_ptr->accept();
					}, detached);
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

	protected:
		detail::io_service_pool io_service_pool_;

		detail::signal_set signals_;

		acceptor acceptor_;

		std::string server_name_;

		std::tuple<Adaptor...> adaptors_;
	};
} // namespace aquarius