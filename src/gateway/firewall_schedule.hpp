#pragma once
#include "firewall_module.h"
#include <aquarius/module/schedule.hpp>

namespace aquarius
{
	namespace gateway
	{
		template <typename R>
		inline auto mpu_firewall_check_ip(const std::string& ip) -> awaitable<R>
		{
			co_return co_await mpu::call<bool, fire_op>("firewall", [&](std::shared_ptr<fire_op> ptr)
														{ return ptr->check_ip(ip); });
		}

		template <typename R>
		inline auto mpu_firewall_check_mac(const std::string& mac) -> awaitable<R>
		{
			co_return co_await mpu::call<bool, fire_op>("firewall", [&](std::shared_ptr<fire_op> ptr)
														{ return ptr->check_mac(mac); });
		}
	} // namespace gateway
} // namespace aquarius