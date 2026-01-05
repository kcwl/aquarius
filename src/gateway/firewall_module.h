#pragma once
#include <aquarius/module/module.hpp>
#include <string>
#include <vector>

namespace aquarius
{
	namespace gateway
	{
		class fire_op
		{
		public:
			fire_op(std::vector<std::string>& ip_addrs, std::vector<std::string>& macs);

			virtual ~fire_op() = default;

		public:
			bool check_ip(const std::string& ip);

			bool check_mac(const std::string& mac);

		private:
			std::vector<std::string>& ip_addrs_;

			std::vector<std::string>& macs_;
		};

		class firewall_module : public no_config_module<int>
		{
		public:
			firewall_module() = default;

		public:
			virtual bool init() override;

		private:
			void get_ip_addrs();

			void get_macs();

		private:
			std::vector<std::string> ip_addrs_;

			std::vector<std::string> macs_;

			std::shared_ptr<fire_op> fire_op_ptr_;
		};
	} // namespace gateway
} // namespace aquarius