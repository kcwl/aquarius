#pragma once
#include <aquarius/singleton.hpp>
#include <aquarius/executor/tcp_client.hpp>

namespace srdh
{
	class client : public aquarius::tcp_client, public aquarius::singleton<client>
	{
	public:

	};
}

#define CLIENT srdh::client::get_mutable_instance()