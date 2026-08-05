#pragma once
#include "gate_error_code.h"
#include "payload.hpp"
#include <aquarius.hpp>

namespace aquarius
{
	namespace gateway
	{
		AQUARIUS_MODULE(srvd_bridge)
		{
		public:
			srvd_bridge();

		public:
			virtual bool init() override;

			virtual auto run() -> asio::awaitable<bool> override;

			auto set_topics(const std::vector<std::string>& topics) -> asio::awaitable<bool>;

		private:
			std::pair<std::string, int32_t> instance_to_host(uint64_t host_and_port);

		private:
			std::string group_;

			std::string srv_host_;

			int32_t srv_port_;
		};
	} // namespace gateway
} // namespace aquarius