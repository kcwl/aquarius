#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/detail/make_endpoint.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/io_service_pool.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/module/http_config_module.hpp>
#include <aquarius/module/module_router.hpp>
#include <aquarius/module/player_module.hpp>
#include <aquarius/module/session_module.hpp>
#include <aquarius/module/sql_module.hpp>
#include <aquarius/timer.hpp>

using namespace std::chrono_literals;

namespace aquarius
{
	template <typename Session>
	class basic_server
	{
	public:
		using session_type = Session;

		using acceptor = typename Session::acceptor;

		using endpoint = typename acceptor::endpoint_type;

		using ip_filter_func = std::function<bool(const std::string&)>;

		using callback_func = std::function<aquarius::awaitable<void>(std::shared_ptr<Session>)>;

	public:
		explicit basic_server(int32_t port, int32_t io_service_pool_size, const std::string& name = {},
							  std::chrono::milliseconds global_time_dura = 30ms)
			: io_service_pool_size_(io_service_pool_size)
			, io_service_pool_(io_service_pool_size_)
			, signals_(io_service_pool_.get_io_service(), SIGINT, SIGTERM)
			, acceptor_(io_service_pool_.get_io_service(), detail::make_v4_endpoint(static_cast<uint16_t>(port)))
			, server_name_(name)
			, global_timer_(io_service_pool_.get_io_service(), global_time_dura)
		{
			init_signal();

			init_global_timer();

			regist_module();

			co_spawn(acceptor_.get_executor(), start_accept(), detached);

			module_router::get_mutable_instance().run();
		}

		~basic_server() = default;

	public:
		void run()
		{
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

		void set_ip_filter(const ip_filter_func& func)
		{
			ip_filter_ = func;
		}

		bool ip_filter(const std::string& host)
		{
			if (!ip_filter_)
				return true;

			return ip_filter_(host);
		}

		void set_accept_func(const callback_func& func)
		{
			accept_func_ = func;
		}

		auto accept_func(std::shared_ptr<session_type> session) -> awaitable<void>
		{
			if (!accept_func_)
				co_return;

			co_await accept_func_(session);
		}

		void set_close_func(const callback_func& func)
		{
			close_func_ = func;
		}

		auto close_func(std::shared_ptr<session_type> session) -> awaitable<void>
		{
			if (!close_func_)
				co_return;

			co_await close_func_(session);
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

				auto session_ptr = std::make_shared<session_type>(std::move(sock), global_timer_.dura(), 3s);

				//co_await mpc_insert_session(session_ptr);

				co_spawn(
					acceptor_.get_executor(),
					[session_ptr, this] -> awaitable<void>
					{
						if (!ip_filter(session_ptr->remote_address()))
						{
							session_ptr->close();
							co_return;
						}

						co_await accept_func(session_ptr);

						auto ec = co_await session_ptr->accept();

						if (ec)
						{
							close_func(session_ptr);
						}

						session_ptr->close();
					},
					detached);
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
			global_timer_.async_wait(
				[this](error_code ec)
				{
					if (ec)
					{
						XLOG_ERROR() << "globale timer error: " << ec.what();
						return;
					}

					module_router::get_mutable_instance().timer(global_timer_.dura());
				});
		}

		void regist_module()
		{
			module_router::get_mutable_instance().regist<session_module<Session>>();

			module_router::get_mutable_instance().regist<sql_module>();

			module_router::get_mutable_instance().regist<http_config_module>();

			module_router::get_mutable_instance().regist<player_module>();
		}

	protected:
		std::size_t io_service_pool_size_;

		io_service_pool io_service_pool_;

		signal_set signals_;

		acceptor acceptor_;

		std::string server_name_;

		timer<steady_timer> global_timer_;

		ip_filter_func ip_filter_;

		callback_func accept_func_;

		callback_func close_func_;
	};
} // namespace aquarius