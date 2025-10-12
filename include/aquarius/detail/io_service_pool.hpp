#pragma once
#include <aquarius/io_context.hpp>
#include <aquarius/logger.hpp>
#include <functional>
#include <list>
#include <thread>
#include <vector>

namespace aquarius
{
	namespace detail
	{
		class io_service_pool
		{
		private:
			using io_service_ptr_t = std::shared_ptr<io_context>;

			using work_guard = boost::asio::executor_work_guard<io_context::executor_type>;

		public:
			explicit io_service_pool(std::size_t pool_size)
				: io_services_()
				, pool_size_(pool_size)
				, next_to_service_(0)
				, works_()
			{
				if (pool_size == 0)
					throw std::runtime_error("io_service_pool size is 0");

				for (std::size_t i = 0; i < pool_size_; ++i)
				{
					io_service_ptr_t io_service_ptr(new io_context{});

					io_services_.push_back(io_service_ptr);

					works_.push_back(make_work_guard(*io_service_ptr));
				}
			}

		public:
			void run()
			{
				std::vector<std::shared_ptr<std::thread>> threads;

				for (auto& io_service : io_services_)
				{
					threads.push_back(std::make_shared<std::thread>(
						[&]
						{
							io_service->run();
						}));
				}

				for (auto& thread : threads)
				{
					thread->join();
				}
			}

			void stop()
			{
				for (auto& io_service : io_services_)
				{
					io_service->stop();
				}
			}

			io_context& get_io_service()
			{
				io_context& io_service = *io_services_[next_to_service_];
				++next_to_service_;

				if (next_to_service_ == io_services_.size())
					next_to_service_ = 0;

				return io_service;
			}

		private:
			io_service_pool(const io_service_pool&) = delete;

			io_service_pool& operator=(const io_service_pool&) = delete;

		private:
			std::vector<io_service_ptr_t> io_services_;

			std::size_t pool_size_;

			std::size_t next_to_service_;

			std::list<work_guard> works_;
		};
	} // namespace detail
} // namespace aquarius

#define IO_POOL aquarius::detail::io_service_pool::get_mutable_instance()