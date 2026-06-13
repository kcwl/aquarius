#pragma once
#include <aquarius/cmd_options.hpp>
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/make_endpoint.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/io_service_pool.hpp>
#include <aquarius/ip/session_store.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/module/module_router.hpp>
#include <aquarius/module/schedule.hpp>

using namespace std::chrono_literals;

namespace aquarius
{
	template <typename Session>
	class basic_server
	{
	public:
		using session_type = Session;

		using acceptor_type = typename session_type::acceptor_type;

		using callback_func = std::function<asio::awaitable<void>(std::shared_ptr<session_type>)>;

	public:
		explicit basic_server(int32_t port, int32_t io_service_pool_size, const std::string& name = {},
							  std::chrono::milliseconds global_time_dura = 30ms)
			: io_service_pool_(io_service_pool_size)
			, signals_(io_service_pool_.get_io_service(), SIGINT, SIGTERM)
			, acceptor_(io_service_pool_.get_io_service(), detail::make_endpoint(static_cast<uint16_t>(port)))
			, server_name_(name)
			, global_time_dura_(global_time_dura)
			, global_timer_(io_service_pool_.get_io_service(), global_time_dura_)
			, keep_alive_(true)
			, nodelay_(false)
		{}

		~basic_server() = default;

	public:
		void run()
		{
			parse_env();

			init_signal();

			asio::co_spawn(acceptor_.get_executor(), start_accept(), asio::detached);

			auto& io = io_service_pool_.get_io_service();
			asio::co_spawn(
				io,
				[&]() -> asio::awaitable<void>
				{
					XLOG_INFO() << "server module router start";
					co_await module_router::get_mutable_instance().run(io_service_pool_);
				},
				asio::detached);

			asio::co_spawn(
				io,
				[&]() -> asio::awaitable<void>
				{
					XLOG_INFO() << "server module global timer start";
					co_await init_global_timer();
				},
				asio::detached);

			XLOG_INFO() << "[run] server run start";

			io_service_pool_.run();
		}

		void stop()
		{
			acceptor_.close();

			io_service_pool_.stop();
		}

		void set_accept_func(const callback_func& func)
		{
			accept_func_ = func;
		}

		auto accept_func(std::shared_ptr<session_type> session) -> asio::awaitable<void>
		{
			if (!accept_func_ || !session)
				co_return;

			XLOG_INFO() << "invoke accept function";
			co_await accept_func_(session);
		}

		void set_close_func(const callback_func& func)
		{
			close_func_ = func;
		}

		auto close_func(std::shared_ptr<session_type> session) -> asio::awaitable<void>
		{
			if (!close_func_ || !session)
				co_return;

			XLOG_INFO() << "invoke close function";
			co_await close_func_(session);
		}

	private:
		auto start_accept() -> asio::awaitable<void>
		{
			error_code ec;
			XLOG_INFO() << "accpent sessions start";
			for (;;)
			{
				auto sock = co_await acceptor_.async_accept(io_service_pool_.get_io_service(),
															asio::redirect_error(asio::use_awaitable, ec));

				if (ec)
					break;

				auto session_ptr = std::make_shared<session_type>(std::move(sock), global_time_dura_);

				session_ptr->keep_alive(keep_alive_);

				session_ptr->nodelay(nodelay_);

				XLOG_INFO() << "create session" << session_ptr->uuid() << "] success";

				mpc_put_session(
					session_ptr->uuid(),
					[session_ptr](flex_buffer& buffer, const std::string&, uint32_t) -> asio::awaitable<void>
					{ co_await session_ptr->async_send_with_header(buffer); });

				asio::co_spawn(
					acceptor_.get_executor(),
					[session_ptr, this] -> asio::awaitable<void>
					{
						co_await accept_func(session_ptr);

						auto ec = co_await session_ptr->accept();

						if (ec)
						{
							XLOG_ERROR() << "session[" << session_ptr->uuid() << "] is close";
						}

						close_func(session_ptr);

						session_ptr->close();
					},
					asio::detached);
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

					XLOG_INFO() << "[server] " << server_name_ << " is stop! signal: " << signal;
				});
		}

		auto init_global_timer() -> asio::awaitable<error_code>
		{
			error_code ec{};
			for (;;)
			{
				co_await global_timer_.async_wait(asio::redirect_error(asio::use_awaitable, ec));

				if (ec)
				{
					XLOG_ERROR() << "globale timer error: " << ec.what();
					break;
				}

				co_await module_router::get_mutable_instance().timer(global_time_dura_);
			}

			co_return ec;
		}

		void parse_env()
		{
			try
			{
				boost::program_options::options_description config("ENV_CFG");
				config.add_options()("keepalive", "socket keep alive")("nodelay", "tcp nodelay");

				boost::program_options::variables_map vm;
				store(boost::program_options::parse_environment(config,
																boost::function1<std::string, std::string>(mapper)),
					  vm);
				notify(vm);

				if (vm.count("keepalive"))
				{
					keep_alive_ = vm["keepalive"].as<bool>();
					XLOG_INFO() << "set keep-alive=" << std::boolalpha << keep_alive_;
				}

				if (vm.count("nodelay"))
				{
					nodelay_ = vm["nodelay"].as<bool>();
					XLOG_INFO() << "set nodelay=" << std::boolalpha << nodelay_;
				}
			}
			catch (std::exception& ptr)
			{
				XLOG_WARNING() << "parse env failed! " << ptr.what();
			}
		}

		static std::string mapper(std::string env_var)
		{
			std::transform(env_var.begin(), env_var.end(), env_var.begin(),
						   [](char c) { return static_cast<char>(std::toupper(static_cast<int>(c))); });

			if (env_var == "KEEPALIVE")
				return "keepalive";
			if (env_var == "NODELAY")
				return "nodelay";
			return "";
		}

	protected:
		io_service_pool io_service_pool_;

		asio::signal_set signals_;

		acceptor_type acceptor_;

		std::string server_name_;

		std::chrono::milliseconds global_time_dura_;

		asio::steady_timer global_timer_;

		callback_func accept_func_;

		callback_func close_func_;

		bool keep_alive_;

		bool nodelay_;
	};
} // namespace aquarius