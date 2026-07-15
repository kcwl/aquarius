#pragma once
#include "gate_error_code.h"
#include "payload.hpp"
#include <aquarius.hpp>

namespace aquarius
{
	namespace gateway
	{
		AQUARIUS_MODULE(client_pool)
		{
			constexpr static std::size_t max_connection = 2;

		public:
			client_pool();

		public:
			virtual auto run() -> asio::awaitable<bool> override;

			auto set_topics(const std::vector<std::string>& topics) -> asio::awaitable<void>;

		private:
			std::pair<std::string, int32_t> instance_to_host(uint64_t host_and_port);

		private:
			std::string group_;
		};
	} // namespace gateway
} // namespace aquarius