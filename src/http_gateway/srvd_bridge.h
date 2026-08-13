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
			std::string group_;

			std::string srvd_host_;

			int32_t srvd_port_;

			std::string host_;

			int16_t port_;

			int32_t weight_;

			std::string version_;

			std::string local_server_name_;
		};

	} // namespace gateway
} // namespace aquarius