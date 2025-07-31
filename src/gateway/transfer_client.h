#pragma once
#include <aquarius/executor/tcp_client.hpp>
#include <aquarius/singleton.hpp>

namespace gateway
{
	class transfer_client : public aquarius::tcp_client, public aquarius::singleton<transfer_client>
	{
	public:
		void set_addr(const std::string& ip_addr);

	private:
		std::string ip_addr_;

		std::string port_;
	};
} // namespace gateway

#define TRANSFER gateway::transfer_client::get_mutable_instance()