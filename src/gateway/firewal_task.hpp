#pragma once

namespace aquarius
{
	namespace gateway
	{

		struct ip_check_task
		{
			ip_check_task(std::string ip)
				: ip_(ip)
			{}

			template <typename Core>
			bool operator()(std::shared_ptr<Core> core_ptr)
			{
				return core_ptr->check_ip(ip_);
			}

			std::string ip_;
		};

		struct mac_check_task
		{
			mac_check_task(std::string mac)
				: mac_(mac)
			{}

			template <typename Core>
			bool operator()(std::shared_ptr<Core> core_ptr)
			{
				return core_ptr->check_ip(mac_);
			}

			std::string mac_;
		};
	} // namespace gateway
} // namespace aquarius