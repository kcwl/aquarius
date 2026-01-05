#pragma once
#include <aquarius/ip/tcp/tcp_client.hpp>
#include <aquarius/module/module.hpp>

namespace aquarius
{
	namespace gateway
	{
		class auth_module : public no_config_module<tcp_client>
		{
		public:
			auth_module() = default;

		private:
			std::shared_ptr<tcp_client> client_ptr_;
		};
	} // namespace gateway
} // namespace aquarius