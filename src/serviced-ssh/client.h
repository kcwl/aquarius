#pragma once
#include <aquarius/ip/tcp.hpp>
#include <aquarius/singleton.hpp>

namespace srdh
{
	class client : public aquarius::ip::tcp_client, public aquarius::singleton<client>
	{
	public:

	};
}

#define CLIENT srdh::client::get_mutable_instance()