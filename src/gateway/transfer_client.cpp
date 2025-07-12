#include "transfer_client.h"

namespace gateway
{
	void transfer_client::set_addr(const std::string& ip_addr)
	{
		if (ip_addr.empty())
			return;

		auto pos = ip_addr.find(":");
		if (pos == std::string::npos)
			return;

		ip_addr_ = ip_addr.substr(0, pos);

		port_ = ip_addr.substr(pos + 1);

		async_connect(ip_addr_, port_);
	}
}