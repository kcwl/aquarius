#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <functional>
#include <thread>

namespace aquarius
{
	namespace srv
	{
		class io_service_pool
		{
		private:
			using io_service_ptr_t = std::shared_ptr<boost::asio::io_service>;
			using work_ptr_t = std::shared_ptr<boost::asio::io_service::work>;

		public:
			explicit io_service_pool(int pool_size)
				: io_services_()
				, pool_size_(pool_size)
				, next_to_service_(0)
				, works_()
			{
				if (pool_size == 0)
					throw std::runtime_error("io_service_pool size is 0");

				for (auto i = 0; i < pool_size_; ++i)
				{
					io_service_ptr_t io_service_ptr(new boost::asio::io_service{});
					work_ptr_t work_ptr(new boost::asio::io_service::work(*io_service_ptr));

					io_services_.push_back(io_service_ptr);
					works_.push_back(work_ptr);
				}
			}

		public:
			void run()
			{
				std::vector<std::shared_ptr<std::thread>> threads;
				std::for_each(
					io_services_.begin(), io_services_.end(),
					[&threads](auto iter)
					{ threads.push_back(std::make_shared<std::thread>([it = std::move(iter)] { it->run(); })); });

				std::for_each(threads.begin(), threads.end(), [](auto iter) { iter->join(); });
			}

			void stop()
			{
				std::for_each(io_services_.begin(), io_services_.end(), [](auto iter) { iter->stop(); });
			}

			boost::asio::io_service& get_io_service()
			{
				boost::asio::io_service& io_service = *io_services_[next_to_service_];
				++next_to_service_;

				if (next_to_service_ == io_services_.size())
					next_to_service_ = 0;

				return io_service;
			}

		private:
			std::vector<io_service_ptr_t> io_services_;

			int pool_size_;

			int next_to_service_;

			std::vector<work_ptr_t> works_;
		};
	} // namespace impl
} // namespace aquarius