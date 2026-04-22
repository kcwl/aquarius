#pragma once
#include "customer_base.h"
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/module/module_register.hpp>
#include <map>
#include <shared_mutex>

namespace aquarius
{
	namespace serviced
	{
		AQUARIUS_MODULE(service_center_module)
		{
		public:
			service_center_module() = default;

		public:
			void regist(std::shared_ptr<customer_base> customer_ptr);

			void remove(std::shared_ptr<customer_base> service_ptr);

			std::shared_ptr<customer_base> get_service(const std::string& group);

			auto timer(std::chrono::milliseconds ms) -> asio::awaitable<void> override;

		private:
			std::shared_mutex mutex_;

			std::map<std::string, std::vector<std::shared_ptr<customer_base>>> services_;
		};
	} // namespace serviced
} // namespace aquarius