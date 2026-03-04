#pragma once
#include <aquarius/module/module.hpp>
#include <string>
#include <vector>

namespace aquarius
{
	namespace gateway
	{
		class firewall_module : public no_config_module<firewall_module>
		{
		public:
			firewall_module(io_context& io, const std::string& name);

		public:
			virtual bool init() override;

		public:
			bool check_ip(const std::string& ip);

			bool check_mac(const std::string& mac);

		private:
			void get_ip_addrs();

			void get_macs();

		private:
			std::vector<std::string> ip_addrs_;

			std::vector<std::string> macs_;
		};
	} // namespace gateway
} // namespace aquarius