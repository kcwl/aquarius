#pragma once
#include <aquarius/executor/tcp_server.hpp>

namespace db
{
	class server : public aquarius::tcp_server
	{
	public:
		using base_type = aquarius::tcp_server;

	public:
		server(uint16_t listen_port, int32_t pool_size, const std::string& server_name, const std::string& host,
			   const std::string& user, const std::string& passwd);

	public:
	};
} // namespace db