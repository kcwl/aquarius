#include "firewall_module.h"
#include <aquarius/module/module_router.hpp>

namespace aquarius
{
	namespace gateway
	{
		firewall_module::firewall_module(const std::string& name)
			: no_config_module<firewall_module>(name)
		{

		}

		bool firewall_module::init()
		{
			get_ip_addrs();

			get_macs();

			return true;
		}

		bool firewall_module::check_ip(const std::string& ip)
		{
			return std::find(ip_addrs_.begin(), ip_addrs_.end(), ip) == ip_addrs_.end();
		}

		bool firewall_module::check_mac(const std::string& mac)
		{
			return std::find(macs_.begin(), macs_.end(), mac) == macs_.end();
		}

		void firewall_module::get_ip_addrs()
		{
			return;
		}

		void firewall_module::get_macs()
		{
			return;
		}
	} // namespace gateway
} // namespace aquarius

AQUARIUS_MODULE_NAMESPACE(aquarius::gateway, firewall_module)