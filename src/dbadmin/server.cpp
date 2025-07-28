#include "server.h"

namespace db
{
	server::server(uint16_t listen_port, int32_t pool_size,
				   const std::string& server_name, const std::string& host, const std::string& user, const std::string& passwd)
		: base_type(listen_port, pool_size, server_name)
	{
		//io_service_pool_.sql_set_params(host, user, user);
	}


} // namespace db