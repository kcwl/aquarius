#include "firewall_schedule.h"
#include "firewall_module.h"
#include <aquarius/module/schedule.hpp>

namespace aquarius
{
	namespace gateway
	{
		auto mpu_firewall_check_ip(const std::string& ip) -> awaitable<bool>
		{
			co_return co_await mpc::call<bool, firewall_module>([&](firewall_module* ptr) -> awaitable<bool>
																{ co_return ptr->check_ip(ip); });
		}

		auto mpu_firewall_check_mac(const std::string& mac) -> awaitable<bool>
		{
			co_return co_await mpc::call<bool, firewall_module>([&](firewall_module* ptr) -> awaitable<bool>
																{ co_return ptr->check_mac(mac); });
		}
	} // namespace gateway
} // namespace aquarius