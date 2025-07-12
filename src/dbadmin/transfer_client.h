#pragma once
#include <aquarius/ip/tcp.hpp>
#include <aquarius/singleton.hpp>

namespace gateway
{
	class transfer_client : public aquarius::ip::tcp_client, public aquarius::singleton<transfer_client>
	{
	public:
		void set_addr(const std::string& ip_addr);

	private:
		std::string ip_addr_;

		std::string port_;
	};
} // namespace gateway

#define TRANSFER gateway::transfer_client::get_mutable_instance()