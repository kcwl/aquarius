#pragma once
#include <aquarius/ip/tcp.hpp>

namespace db
{
	class server : public aquarius::ip::tcp_server
	{
	public:
		using base_type = aquarius::ip::tcp_server;

	public:
		server(uint16_t listen_port, int32_t pool_size, const std::string& server_name, const std::string& db_addr, const std::string& db_user, const std::string& db_passwd);

	public:

	};
} // namespace db