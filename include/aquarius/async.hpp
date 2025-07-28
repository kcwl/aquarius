#pragma once
#include <aquarius/concepts/async.hpp>
#include <aquarius/detail/io_service_pool.hpp>
#include <aquarius/client_pool.hpp>

namespace aquarius
{
	class generator
	{
	public:
		generator()
			: service_pool_(get_client_pool_size())
		{

		}

		virtual~generator()
		{
			if (thread_ptr_ && thread_ptr_->joinable())
			{
				thread_ptr_->join();
			}
		}

	public:
		template<typename T>
		std::shared_ptr<T> async(const std::string& host, const std::string& port, int reconnect_times, std::chrono::steady_clock::duration timeout)
		{
			auto client_ptr = std::make_shared<T>(service_pool_.get_io_service(), reconnect_times, timeout);

			co_spawn(service_pool_.get_io_service(), [client_ptr, &host, &port]() ->awaitable<void>
				{
					co_await client_ptr->async_connect(host, port);
				});

			std::call_once(once_, &generator::init_thread, this);

			return client_ptr;
		}

	private:
		void init_thread()
		{
			if (!thread_ptr_ || !thread_ptr_->joinable())
			{
				thread_ptr_ = std::make_shared<std::thread>([&] { this->service_pool_.run(); });
			}
		}

	private:
		std::shared_ptr<std::thread> thread_ptr_;

		detail::io_service_pool service_pool_;

		std::once_flag once_;
	};


	template <client_t T>
	std::shared_ptr<T> async_generator(const std::string& host, const std::string& port, int reconnect_times = 3, std::chrono::steady_clock::duration timeout = 100ms)
	{
		static generator gen{};

		return gen.async<T>(host, port, reconnect_times, timeout);
	}
} // namespace aquarius