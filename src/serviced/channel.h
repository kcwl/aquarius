#pragma once
#include "customer.h"
#include "subscriber.h"
#include <aquarius.hpp>
#include <map>
#include <memory>
#include <vector>

namespace aquarius
{
	namespace serviced
	{
		class channel
		{
		public:
			channel() = default;

		public:
			void regist(const std::string& topic, std::shared_ptr<subscriber> subscriber_ptr);

			void regist(std::shared_ptr<customer> customer_ptr);

			void remove(const std::string& topic, std::shared_ptr<subscriber> subscriber_ptr);

			void remove(std::shared_ptr<customer> customer_ptr);

			auto invoke(const std::string& topic, flex_buffer& buffer) -> asio::awaitable<void>;

			bool empty() const;

			auto get_services(const std::string& topic) -> std::expected<std::vector<std::shared_ptr<customer>>, bool>;

		private:
			std::map<std::string, std::vector<std::shared_ptr<subscriber>>> subscribers_;

			std::map<std::string, std::vector<std::shared_ptr<customer>>> customers_;
		};
	} // namespace serviced
} // namespace aquarius