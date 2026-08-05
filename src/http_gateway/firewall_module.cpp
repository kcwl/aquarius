#include "firewall_module.h"
#include <aquarius/module/module_router.hpp>

namespace aquarius
{
	namespace gateway
	{
		auto firewall_module::check_ip(const std::string& ip) -> asio::awaitable<bool>
		{
			co_return true;
		}

		auto firewall_module::check_mac(const std::string& mac) -> asio::awaitable<bool>
		{
			co_return true;
		}
	} // namespace gateway
} // namespace aquarius