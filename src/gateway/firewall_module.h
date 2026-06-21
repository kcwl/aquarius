#pragma once
#include <aquarius/module/module_register.hpp>
#include <string>

namespace aquarius
{
	namespace gateway
	{
		AQUARIUS_MODULE(firewall_module)
		{
		public:
			firewall_module() = default;

		public:
			auto check_ip(const std::string& ip) -> asio::awaitable<bool>;

			auto check_mac(const std::string& mac) -> asio::awaitable<bool>;
		};
	} // namespace gateway
} // namespace aquarius