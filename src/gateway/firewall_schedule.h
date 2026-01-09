#pragma once
#include <aquarius/asio.hpp>
#include <string>

namespace aquarius
{
	namespace gateway
	{
		auto mpu_firewall_check_ip(const std::string& ip) -> awaitable<bool>;

		auto mpu_firewall_check_mac(const std::string& mac) -> awaitable<bool>;
	} // namespace gateway
} // namespace aquarius