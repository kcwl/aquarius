#include "firewall_module.h"

namespace aquarius
{
	namespace gateway
	{
		fire_op::fire_op(std::vector<std::string>& ip_addrs, std::vector<std::string>& macs)
			: ip_addrs_(ip_addrs)
			, macs_(macs)
		{}

		bool fire_op::check_ip(const std::string& ip)
		{
			return std::find(ip_addrs_.begin(), ip_addrs_.end(), ip) == ip_addrs_.end();
		}

		bool fire_op::check_mac(const std::string& mac)
		{
			return std::find(macs_.begin(), macs_.end(), mac) == macs_.end();
		}

		bool firewall_module::init()
		{
			get_ip_addrs();

			get_macs();

			fire_op_ptr_ = std::make_shared<fire_op>(ip_addrs_, macs_);

			return fire_op_ptr_ != nullptr;
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